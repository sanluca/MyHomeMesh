#include <Servo.h>
Servo s;

String text1 = "1";
String text2 = "2";
String text3 = "3";
String text4 = "4";
String text5 = "5";
String text6 = "10";
String text7 = "7";
String text8 = "8";
String text9 = "9";

String phone1 = "forwardStart\n";
String phone2 = "backwardStart\n";
String phone3 = "leftStart\n";
String phone4 = "rightStart\n";
String phone5 = "aStart\n";
String phone6 = "bStart\n";

String phone7 = "forwardStop\n";
String phone8 = "backwardStop\n";
String phone9 = "leftStop\n";
String phone10 = "rightStop\n";
String comdata = "";
char judge;
void setup()
{
   s.attach(2);
   s.write(90);
   Serial.begin(115200);       // set up a wifi serial communication baud rate 115200
   
   Serial.println("AT+CWMODE=3\r\n");//set to softAP+station mode
   delay(1000);     //delay 4s  
   Serial.println("AT+CWSAP=\"MY_ESP8266\",\"12345678\",8,2\r\n");   //TCP Protocol, server IP addr, port
    delay(1000);     //delay 4s
   Serial.println("AT+RST\r\n");     //reset wifi
   delay(1000);     //delay 4s
   Serial.println("AT+CIPMUX=1\r\n");//set to multi-connection mode
   delay(1000);
   Serial.println("AT+CIPSERVER=1\r\n");//set as server
   delay(1000);
   Serial.println("AT+CIPSTO=7000\r\n");//keep the wifi connecting 7000 seconds
   delay(1000);
}
int angle = 90;
void loop()
{
    while(Serial.available()>0)
   {  
    comdata += char(Serial.read());
    delay(1);
   }
   judgement();
   switch(judge)
   {
      case 1: {angle+=5;judge = 0;}
               break;
      case 2: {angle-=5;judge = 0;} 
               break; 
   }
   s.write(angle);
}

void judgement(){
    if (comdata.length() > 0)
    { if(comdata.endsWith(text1)||comdata.endsWith(phone1)){//forward
          judge=1;
        
        }
        if(comdata.endsWith(text2)||comdata.endsWith(phone2)){//backward
          judge=2;
        }
        if(comdata.endsWith(text3)||comdata.endsWith(phone3)){//right
          judge=3;
        }
        if(comdata.endsWith(text4)||comdata.endsWith(phone4)){//left
          judge=4;
        }
        if(comdata.endsWith(text5)||comdata.endsWith(phone5)){//balance
          judge=5;
        }
        if(comdata.endsWith(text6)||comdata.endsWith(phone6)){//aviod
          judge=6;
        }
        if(comdata.endsWith(text7))
        {
            judge=7;
        }
        if(comdata.endsWith(phone7)||comdata.endsWith(phone8)||comdata.endsWith(phone9)||comdata.endsWith(phone10)){
          judge=11;//初始位置
        }
        if(comdata==text8){
          judge=8;
        }
        if(comdata.endsWith(text9)){
          judge=9;
        }
        if(comdata.endsWith(text5))
        {
          ;//A
        }
        if(comdata.endsWith(text6))
        {
          ;//B
        }
        comdata = "";
        delay(10);
    }
}
