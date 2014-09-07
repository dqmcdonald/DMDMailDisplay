
/*
DMDMailDisplay


Fetch Mail from a Gmail account and display it on a FreeTronics DMD LED Matrix display. Mail is actually fetched via
a Python script.


Uses Dragino Yun Shield (http://www.dragino.com/products/yunshield/item/86-yun-shield.html) and Freetronics DMD
LED Matrix (http://www.freetronics.com/products/dot-matrix-display-32x16-red#.VAzpwGS1YY4).


Also listen for a serial connection on PIN 15. This should be a byte that represents a message ID


Maximum length of email message is 128 characters.


Quentin McDonald
September 2014

*/


#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <Process.h>
#include<Bridge.h>
#include <Console.h>
#include <SoftwareSerial.h>

const uint8_t *FONT =  Arial14;
SPIDMD dmd(1, 1); // DMD controls the entire display

// This script will be copied to the YunShield when uploaded
#define SCRIPT "/mnt/sda1/arduino/www/DMDMailDisplay/DMDMailDisplay.py"

#define CHARACTER_DELAY 150   //  delay after printing each character on the screen
#define TEXT_DELAY 1000       //  delay after printing each text before the screen is cleared

const char* FETCHING_EMAIL_MESSAGE = "Fetching...";
const char* DONE_EMAIL_MESSAGE = "Done";

int message_serial_num = 0; // Give each message a unique serial number in the subject line


const char* messages[] = {
  "Hi There",
  "Thanks",
  "Love you",
  "Miss you too"
};


// Uncomment the following line to get debugging messages printed to the Console:
//#define DEBUG 1

#define RX_PIN 15
#define TX_PIN 16
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX,TX

// This buffer determines the max. size of the email message.
char buff[128];

int i = 0;
char c;
char c1, c2;


unsigned long  check_mail_now = 1;
const unsigned long CHECK_MAIL_PERIOD = 180000; // Check every three minutes

unsigned long display_message_now = 1;
const unsigned long DISPLAY_MESSAGE_PERIOD = 20000; // Display every 20 seconds


void setup() {


  Bridge.begin();


#ifdef DEBUG
  Console.begin();
  while (!Console) {
    ; // wait for Console port to connect.
  }
  Console.println("Beginning");
#endif

  dmd.setBrightness(100);
  dmd.selectFont(FONT);
  dmd.begin();

  Bridge.put("DMDTODISP", "Nothing");

  mySerial.begin(4800);
  mySerial.flush();
}


void loop() {

  // Check for new mail from time to time:
  if ( millis() > check_mail_now ) {
#ifdef DEBUG
    Console.println("Fetching mail");
#endif
    displayText(FETCHING_EMAIL_MESSAGE);
    check_mail_now = millis() + CHECK_MAIL_PERIOD;
    Process p;
    p.begin("python");
    p.addParameter(SCRIPT);
    p.addParameter("FETCH");
    p.run();
    displayText(DONE_EMAIL_MESSAGE);
#ifdef DEBUG
    Console.println("Done fetching mail");
#endif
  }


  // Display the message from time to time:
  if ( millis() > display_message_now ) {


    Bridge.get("DMDTODISP", buff, 128 );
    displayText( buff);
    display_message_now = millis() + DISPLAY_MESSAGE_PERIOD;
  }

  delay(500);  // Delay is simply to avoid wasting any cycles calling all this too ofen


  // Check for Serial bytes on pin 15.
  if ( mySerial.available() ) {
    int uid = (int)mySerial.read();
#ifdef DEBUG
    Console.print("Found Card: " );
    Console.println( uid, DEC );
#endif
    int msg_index = getMessageIndex(uid);
#ifdef DEBUG
    Console.print("Message Index: " );
    Console.println( msg_index, DEC );
#endif
    if ( msg_index >= 0 ) {
      sendEmail( msg_index );

    }
  }

}


// Display a line, character by character on the LED matrix:

void displayText( const char * txt ) {
  DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display

  dmd.clearScreen();
  for ( unsigned j = 0; j < strlen(txt); j++ ) {
    box.print(txt[j]);
    delay(CHARACTER_DELAY);
  }
  delay(TEXT_DELAY);
  dmd.clearScreen();
}


// Convert a RFID ID into a message index, an index into the array of possible messages:
int getMessageIndex( int uid ) {
  // Convert RFID Card UID into message index:
  int idx = -1;

  switch ( uid ) {
    case 244:
      idx = 0;
      break;
    case 84:
      idx = 1;
      break;
    case 37:
      idx = 2;
      break;
    case 205:
      idx = 3;
      break;
    default:
      idx = -1;
  }

  return idx;

}


// Send an email based on the message index 'idx'
void sendEmail( int idx ) {

#ifdef DEBUG
  Console.println("Sending mail");
#endif
  displayText("Sending...");
  displayText(messages[idx]);
  Bridge.put("TOEMAIL", messages[idx]);
  message_serial_num += 1;
  sprintf(buff,"%d", message_serial_num );
  Bridge.put("EMAILSER", buff);
  Process p;
  p.begin("python");
  p.addParameter(SCRIPT);
  p.addParameter("SEND");
  p.run();
#ifdef DEBUG
  Console.println("Done sending mail");
#endif
  displayText("Message sent");

}


