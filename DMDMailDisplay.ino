

#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <Process.h>
#include<FileIO.h>
#include<Bridge.h>

const uint8_t *FONT =  Arial14;
SoftDMD dmd(1, 1); // DMD controls the entire display

#define SCRIPT "/mnt/sda1/arduino/www/DMDMailDisplay/DMDMailDisplay.py"

char buff[128];

int i = 0;
char c;
unsigned long  check_mail_now = 1;
const unsigned long CHECK_MAIL_PERIOD = 120000; // Check every two minutes

unsigned long display_message_now = 1;
const unsigned long DISPLAY_MESSAGE_PERIOD = 10000; // Display every 10 seconds


void setup() {
  Bridge.begin();
  // put your setup code here, to run once:

  dmd.setBrightness(100);
  dmd.selectFont(FONT);
  dmd.begin();

  FileSystem.begin();

  Bridge.put("DMDTODISP", "Nothing");

}


void loop() {

  // Check for new mail from time to time:
  if ( millis() > check_mail_now ) {
    check_mail_now = millis() + CHECK_MAIL_PERIOD;
    Process p;
    p.begin("python");
    p.addParameter(SCRIPT);
    p.run();
  }

  if ( millis() > display_message_now ) {
    DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display

 
    Bridge.get("DMDTODISP", buff, 128 );
    for ( i = 0; i < strlen(buff); i++ ) {

      box.print(buff[i]);
      delay(200);
    }



    delay(1000);
    dmd.clearScreen();



    display_message_now = millis() + DISPLAY_MESSAGE_PERIOD;
  }


}
