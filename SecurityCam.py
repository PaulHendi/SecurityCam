# Opencv library
import cv2

# To get the current date/time
import datetime
import time

# To use bash function (need a unix based OS)
import os

# Serial communication libraries
from bluetooth import *
import serial

# Email libraries
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
from email.mime.image import MIMEImage




# Those lines below need to be changed
email_sender = 'blabla@blabla.com'
pwd_sender = '123456789'
email_receiver = 'bloblo@bloblo.com'



def sendMail(imageFolder):


    # For more details about sending an e-mail, check the python library email

    # Setting up the MIMEMultipart()
    msg = MIMEMultipart()
    msg['From'] = email_sender
    msg['To'] = email_receiver
    msg['Subject'] = 'Message from your house' 

    # Sending a text message, you can change it to whatever you like
    message = 'Someone is here, check the picture :'
    msg.attach(MIMEText(message))

    # We send the third picture taken by the camera but again you can change it
    img_data = open(imageFolder + "3.jpg", 'rb').read()
    image = MIMEImage(img_data, name=os.path.basename(imageFolder + "3.png"))
    msg.attach(image)

    # Finally sending the e-mail
    mailserver = smtplib.SMTP('smtp.live.com', 587)
    mailserver.ehlo()
    mailserver.starttls()
    mailserver.ehlo()
    mailserver.login(email_sender, pwd_sender)
    mailserver.sendmail(email_sender, email_receiver, msg.as_string())
    mailserver.quit()



# We should bind it at the beginning
os.system("sudo rfcomm bind rfcomm0 20:13:12:05:17:41") # Put the MAC adress of your bluetooth device 
os.system("sleep 10")


# Supposing we could connect to the bluetooth 
while(True) :


    # Reading the data coming from the arduino
    ser = serial.Serial('/dev/rfcomm0', 115200)
    data = ser.read()

    
    # If someone has been detected at less than 1m50 in from of the ultrasound sensor
    if int(data) == 1 :

        print("Someone has been detected")

        # Get current date
        CurrentDate =  str(datetime.datetime.today()).split(" ")
        date = CurrentDate[1] + "--" + CurrentDate[0] 

        # Make a folder (bash commands)
        os.system("mkdir Images/" + date)
        Folder = "Images/" + date + "/"

        # Start capturing a video from the main camera, usually a webcam from a laptop
        cap = cv2.VideoCapture(0)


        # Save all images to the folder that you created using opencv
        tstart = time.time()
        t = 0.0
        i = 0
        while(t < 5.0) :  # Capture 5 seconds only
            check, frame = cap.read()
            if check :
                cv2.imwrite(Folder + str(i) + ".jpg", frame)
            cv2.waitKey(1)
            i+=1
            t = time.time() - tstart


        cap.release()

        # Send an email to the user chosen 
        sendMail(Folder)


# Close the communication with the arduino 
serial.Serial('/dev/rfcomm0').close()
