
#!/usr/bin/env python

import sys
import imaplib
import email
import private as private


# Note: This function definition needs to be placed
#       before the previous block of code that calls it.
def process_mailbox(M):
  rv, data = M.search(None,'All')
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


      f=open("/tmp/todisp.txt",'w');                    
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
            f.write(l);                                        
            f.write('\n');                                     
                                                               
      f.close();            
      


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


