/***********************************************************
File name: 02_servo.ino
Description:   The servo motor circulates to 180 degrees, 90 degrees
               Degrees, 0 degrees,
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2020/06/09 
***********************************************************/
#include <Servo.h>
Servo myservo;//create servo object to control a servo
void setup()
{
  myservo.attach(3);//attachs the servo on pin 2 to servo object
  myservo.write(0);//back to 0 degrees 
  delay(1000);//wait for a second
}
void loop()
{  
  myservo.write(180);//goes to 15 degrees 
  delay(2000);//wait for a second

  myservo.write(90);//goes to 30 degrees 
  delay(2000);//wait for a second.
  
  myservo.write(0);//goes to 45 degrees 
  delay(2000);//wait for a second.
}
