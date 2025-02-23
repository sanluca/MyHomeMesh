/***********************************************************
File name: WIFI_control.ino
Description:Robot motion can be controlled through mobile APP or Python GUI. 
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2020/05/22
***********************************************************/
#include "hexpod.h"
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
String phone3 = "rightStart\n";
String phone4 = "leftStart\n";
String phone5 = "aStart\n";
String phone6 = "bStart\n";

String phone7 = "forwardStop\n";
String phone8 = "backwardStop\n";
String phone9 = "leftStop\n";
String phone10 = "rightStop\n";

String comdata = "";
char judge;
QUANRUPED q;
void ESP8266_ATCOMMAND(){
   Serial.begin(115200);       // set up a wifi serial communication baud rate 115200
//   pinMode(controller, OUTPUT);    // sets the RelayPin to be an output
   Serial.println("AT+CWMODE=3\r\n");//set to softAP+station mode
   delay(2000);     //delay 4s
   
   Serial.println("AT+CWSAP=\"Adeept_ESP\",\"12345678\",8,2\r\n");   //TCP Protocol, server IP addr, port
    delay(2000);     //delay 4s
   Serial.println("AT+RST\r\n");     //reset wifi
   delay(2000);     //delay 4s
  
   Serial.println("AT+CIPMUX=1\r\n");
   delay(2000);
   Serial.println("AT+CIPSERVER=1\r\n");
   delay(2000);
   Serial.println("AT+CIPSTO=7000\r\n");
   delay(2000);
}

void setup()
{
   q.servo_attach();
   q.strip_begin(255,255,255);
   ESP8266_ATCOMMAND();
   q.self_balanced_setup();
   q.strip_begin(50,100,255);
}
void loop()
{
   while(Serial.available()>0)
   {  
    comdata += char(Serial.read());
    delay(1);
   }
   judgement();
   switch(judge){
     case 1: {q.moveforward();}
               break;
     case 2: {q.movebackward();} 
               break;
     case 3: {q.turnright();} 
               break;
     case 4: {q.turnleft();}
               break; 
     case 5: {q.steaty();}
               break; 
     case 6: {q.attack();} 
               break;
     case 8: {q.advoid();} 
               break;
       
     case 9: {q.servo_init();
     } 
               break;      
     default:  break;
  }
   if(judge==7){                          //send the ultrasonic data to ESP8266
      Serial.println("AT+CIPSEND=0,4\r\n");
      delay(20);
      Serial.println("U:35");
      delay(250);
      for(int u=0;u<104;u++){
      q.sendultrasonic();
      }
      Serial.println("AT+CIPSEND=0,1\r\n");
      delay(20);
      Serial.println("F");
      delay(250);
      
      judge=0;
     }

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
        if(comdata.endsWith(text8)){
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
