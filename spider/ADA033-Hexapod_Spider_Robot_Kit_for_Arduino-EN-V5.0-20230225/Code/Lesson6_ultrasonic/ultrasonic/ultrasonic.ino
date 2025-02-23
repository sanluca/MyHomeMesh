/***********************************************************
File name: _05_Ultrasonic.ino
Description: When you move the obstacle in front of the 
             ultrasonic module,you can see the data on 
             the serial montiol.
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2017/03/18 
***********************************************************/
const int pingPin = A3;  // pin connected to Echo Pin in the ultrasonic distance sensor
const int trigPin = A2;  // pin connected to trig Pin in the ultrasonic distance sensor

void setup() 
{
   pinMode(pingPin, INPUT); //Set the connection pin output mode Echo pin
   pinMode(trigPin, OUTPUT);//Set the connection pin output mode trog pin
   Serial.begin(9600);     //opens serial port, sets data rate to 9600 bps
} 

void loop() 
{ 
   int cm = ping(pingPin); 
   Serial.print("distance: "); // Print a message of "Temp: "to the serial montiol.
   Serial.print(cm);           // Print a centigrade temperature to the serial montiol. 
   Serial.println(" cm");      // Print the unit of the centigrade temperature to the serial montiol.
   delay(500);
}
     
int ping(int pingPin) 
{ 
   // establish variables for duration of the ping, 
   // and the distance result in inches and centimeters: 
   long duration, cm; 
   // The PING))) is triggered by a HIGH pulse of 2 or more microseconds. 
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse: 
   pinMode(trigPin, OUTPUT); 
   digitalWrite(trigPin, LOW); 
   delayMicroseconds(2); 
   digitalWrite(trigPin, HIGH); 
   delayMicroseconds(5); 
   digitalWrite(trigPin, LOW); 

   pinMode(pingPin, INPUT); 
   duration = pulseIn(pingPin, HIGH); 

   // convert the time into a distance 
   cm = microsecondsToCentimeters(duration); 
   return cm ; 
} 

long microsecondsToCentimeters(long microseconds) 
{ 
   // The speed of sound is 340 m/s or 29 microseconds per centimeter. 
   // The ping travels out and back, so to find the distance of the 
   // object we take half of the distance travelled. 
   return microseconds / 29 / 2; 
} 

        
        
        
        
      
