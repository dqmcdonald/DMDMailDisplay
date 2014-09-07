#
#  Python script to fetch and send mail.
#
#!/usr/bin/env python

import sys
import imaplib
import email
import smtplib


# Note this module is not uploaded to github. It needs to define:
#EMAIL=<gmail user name>
#PASSWORD=<gmail password>
#FROM=<the only address messages will be displayed from>
#TO=<where email messsages should be sent to>
import private as private  

# We need to find the bridge module:
sys.path.insert(0, '/usr/lib/python2.7/bridge/') 
                                                
from bridgeclient import BridgeClient as bridgeclient

value = bridgeclient()


# Process the IMAP mail box that's already been opened:
def process_mailbox(M):
  rv, data = M.search(None,'(FROM "%s")' % private.FROM)  # Restrict messages to those from a particular address
  if rv != 'OK':
      print "No messages found!"
      return

  num_lst = data[0].split()
  if len(num_lst) == 0:    
        return;            
  for num in num_lst[-1]:  # Get the latest message:
      rv, data = M.fetch(num, '(RFC822)')
      if rv != 'OK':
          print "ERROR getting message", num
          return

      # Handle both multi-part and single part messages:
      msg = email.message_from_string(data[0][1])
      if msg.is_multipart():                     
        p = msg.get_payload(0)                   
        lines= p.as_string()                     
      else:                                      
        p = msg.get_payload(decode=True)         
        lines = p                                
      for l in lines.split('\n'):                
          if not l.startswith( "Content-Type:") and len(l) > 1:
            print l                                            
            value.put("DMDTODISP",l)       # This will make the message available to teh Arduino                        
                                     
                                                               
              
# Fetch mail from the gmail server     
def fetch_mail():

    M = imaplib.IMAP4_SSL('imap.gmail.com')

    try:
        M.login(private.EMAIL,private.PASSWORD)
    except imaplib.IMAP4.error:
        print "LOGIN FAILED!!! "
        # ... exit or deal with failure...

    rv, data = M.select("INBOX")
    if rv == 'OK':
        process_mailbox(M) # ... do something with emails, see below ...
        M.close()
    M.logout()


def send_mail():
    # These next two values will be set by teh Arduino via the Bridge:
    body=value.get("TOEMAIL")
    ser=value.get("EMAILSER")
    smtpserver = smtplib.SMTP("smtp.gmail.com",587)
    smtpserver.ehlo()
    smtpserver.starttls()
    smtpserver.ehlo
    smtpserver.login(private.EMAIL, private.PASSWORD)
    header = 'To:' + private.TO + '\n' + 'From: ' + private.EMAIL
    header = header + '\n' + 'Subject:' + 'Message from home #%d' % int(ser)  + '\n'
    msg = header + '\n' + body + ' \n\n' + '\n\n'
    print msg
    smtpserver.sendmail(private.EMAIL, private.TO, msg)
    smtpserver.close()


    

    
if __name__ == "__main__":
    if sys.argv[1] == "FETCH":
        fetch_mail()
    elif sys.argv[1] == "SEND":
        send_mail()
