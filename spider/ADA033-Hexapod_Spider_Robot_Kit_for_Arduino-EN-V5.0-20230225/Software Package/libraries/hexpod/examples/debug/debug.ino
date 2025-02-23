/***********************************************************
File name: debug.ino
Description:Adjust steering gear median.. 
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2020/05/22
***********************************************************/
#include <ArduinoJson.h>

#include <Servo.h>
#define leg_data 13
int angle[leg_data] = {90,90,90,90,90,90,90,90,90,90,90,90,90};

Servo myservo[leg_data];
char line[60] = "";// Serial data

void setup()
{
  Serial.begin(115200);
  
  for(int i = 0;i<leg_data;i++)
    myservo[i].attach(i+2);
  for(int i = 0;i<leg_data;i++)
    myservo[i].write(angle[i]);
}
void loop()
{
   DynamicJsonDocument  jsonBuffer(100);

if (Serial.available() > 0) {
  
//  read incoming data: read up to \n, or up to 500 characters
 Serial.readBytesUntil('\n', line, 100);
//Serial.println(line);

  deserializeJson(jsonBuffer, line);
JsonObject root = jsonBuffer.as<JsonObject>();

  my_cmp(root);

memset(line, 0, sizeof(line));
}
}

void my_cmp(JsonObject root)
{
  const char *python_char =  root["start"][0];
  long one = root["start"][1],two = root["start"][2];
   if(strcmp(python_char,"setup")==0)
  {
      Serial.println("succes");
      memset(line, 0, sizeof(line));
  }
  if(strcmp(python_char,"angle")==0)
  {
      
      myservo[one].write(two);
      memset(line, 0, sizeof(line));
  }
}
