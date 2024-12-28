//************************************************************
// read temperature  "birra"
//
//birra
// pin dallas D4 --> 2
//pin rele D8 --> 15
//pin conta bolle D5 --> 14
//
//************************************************************
#include <EEPROM.h>
#include <Wire.h>
#include "Daisy24.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void ICACHE_RAM_ATTR read_bol();

float t;
int termostato;
unsigned long now;
unsigned long m;
unsigned long t_t;
int count=0;
const byte relay = D8;
int bolle = D5;
String rel;
String rel2;
byte i;
#define pcf_add 0x3F

volatile boolean sendBolle=false;
volatile boolean relayState=LOW;

LCD_Daisy24 lcd;
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg;
String nodeName = "birra"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "birra"
#define VERSION "Birra v1.0.19"
#define MESSAGE "birra "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  update_temperature();
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  data_lcd();
  update_status();
  turn_off_lcd();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 100, TASK_SECOND * 150 ));

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
 // rel= msg;
  root_id=from;
  if(msg.toInt()>1){
    termostato= msg.toInt();
    EEPROM.write(0,termostato);
    EEPROM.commit();
    msg = "termostato/";
    msg += termostato;
    mesh.sendSingle(to, msg);
  }
  }

void update_rele(){
  if (rel2 != rel){
 if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay, HIGH);   
      relayState = HIGH;
      msg = "output/";
      msg += "1";
      mesh.sendSingle(to, msg);
      
   }
   else if (strcmp(rel.c_str(),"0") == 0) {
    digitalWrite(relay, LOW);   
    relayState = LOW;
    msg = "output/";
    msg += "0";
    mesh.sendSingle(to, msg);
  } 
  
  rel2=rel;
}}


void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void read_dallas(){
 
  sensors.requestTemperatures();
  t=sensors.getTempCByIndex(0);
  if (isnan(t)) {
          Serial.println("Failed to read from Dallas sensor!");
          msg = "error/";
          msg += "Failed to read from Dallas sensor!";
          mesh.sendSingle(to, msg);
          return;
        } 
 }

void read_bol(){
    sendBolle=true;
  }

void update_status()
{
  long uptime = millis() / 60000L;
  msg = "uptime/";
  msg += uptime;
  mesh.sendSingle(to, msg);
  msg = "nodeid/";
  msg += mesh.getNodeId();
  mesh.sendSingle(to, msg);
  msg = "freememory/";
  msg += String(ESP.getFreeHeap());
  mesh.sendSingle(to, msg);
  msg = "version/";
  msg += VERSION;
  mesh.sendSingle(to, msg);
  msg = "root/";
  msg += root_id;
  mesh.sendSingle(to, msg);
  msg = "output/";
  msg += relayState;
  mesh.sendSingle(to, msg);
  msg = "termostato/";
  msg += termostato;
  mesh.sendSingle(to, msg);
  
}

void start_lcd(){
  lcd.setContrast(38); 
  lcd.print("Daisy24");
  delay(1000);
  lcd.setDoubleFont();
  delay(1000);
  lcd.setSingleFont();
  lcd.clear();
  lcd.print("Controllo Birra"); 

//  for(i = 0; i < 2; i++)
//  {
//    lcd.noDisplay();
//    delay(200);
//    lcd.display();
//    delay(200);
//  }

//for(byte i = 0; i < 2; i++)
//  {
//    delay(200);
//    lcd.lightOFF(pcf_add); 
//    delay(200);
    lcd.lightON(pcf_add);  
//  }
//  lcd.clear();
}

void data_lcd(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp C");
  lcd.setCursor(12,0);
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("Temp Imp C");
  lcd.setCursor(12,1);
  lcd.print(termostato);
  
}

void turn_off_lcd()
{
  if((millis() - m) > 600000) {
    m = millis();
    lcd.lightOFF(pcf_add); 
}}

void update_temperature(){
  read_dallas();
  
  if(t>=termostato){
    digitalWrite(relay, HIGH);   
    relayState = HIGH;
    msg = "output/";
    msg += "1";
    mesh.sendSingle(to, msg);
  }
  else{
    digitalWrite(relay, LOW);   
    relayState = LOW;
    msg = "output/";
    msg += "0";
    mesh.sendSingle(to, msg);
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Wire.begin();
  lcd.begin();  
  pinMode(relay, OUTPUT);
  EEPROM.begin(512);
  termostato=EEPROM.read(0);
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 11 );
  mesh.initOTAReceive(ROLE);
  mesh.setContainsRoot(true);
  mesh.setName(nodeName);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  
  attachInterrupt(digitalPinToInterrupt(bolle), read_bol, FALLING);

  start_lcd();

}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
//  update_rele();
//  turn_off_lcd();
 // update_temperature();
  if((millis() - now) > 600000) { //600000 - 10 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}
 if (sendBolle==true){
  
  msg = "bolle/";
  msg += "1";
  mesh.sendSingle(to, msg);
  sendBolle=false;
  }
}
