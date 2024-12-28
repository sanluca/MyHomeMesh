//************************************************************
// read temperature "acquario"
////pin dallas D4 --> 2
// pin rele D5 --> 14
// pin rele D6 --> 12
//
//************************************************************
#include "Wire.h"  
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const byte relay1 = D5;
const byte relay2 = D6;
bool relayState1 = LOW;
bool relayState2 = LOW;
float temp;
unsigned long now;


//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg;
String rel;
String rel2;
String nodeName = "acquario"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "acquario"
#define VERSION "Acquario v2.0.0"
#define MESSAGE "acquario "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void read_dallas(){
 
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
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
  msg = "ip/";
  msg += WiFi.localIP().toString();
  mesh.sendSingle(to, msg);
  
}

void sendMessage() {
  read_dallas();
  msg = "temperatura/";
  msg += temp;
  mesh.sendSingle(to, msg);

  update_status();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 300, TASK_SECOND * 400 ));

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel = msg;
  root_id=from;

 }

void update_rele(){
  if (rel2 != rel){
   if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay1, HIGH);
      relayState1 = HIGH;
      msg = "output/";
      msg += "1";
      mesh.sendSingle(to, msg);
   }
    else if (strcmp(rel.c_str(),"2") == 0) {
      digitalWrite(relay1, LOW);
      relayState1 = LOW;
      msg = "output/";
      msg += "2";
      mesh.sendSingle(to, msg);
    }
   else if (strcmp(rel.c_str(),"3") == 0) {
    digitalWrite(relay2, HIGH);
    relayState2 = HIGH;
    msg = "output/";
    msg += "3";
    mesh.sendSingle(to, msg);
  } 
  else if (strcmp(rel.c_str(),"4") == 0) {
    digitalWrite(relay2, LOW); 
    relayState2 = LOW;
    msg = "output/";
    msg += "4";
    mesh.sendSingle(to, msg);
  } 

  else if (strcmp(rel.c_str(),"0") == 0) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    relayState1 = LOW;
    relayState2 = LOW;
    msg = "output/";
    msg += "0";
    mesh.sendSingle(to, msg);
  }
   rel2=rel;
  }
 
}


void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  Serial.begin(115200);
  sensors.begin();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 11);
  mesh.initOTAReceive(ROLE);
  mesh.setContainsRoot(true); 
  mesh.setName(nodeName);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  update_rele();
 if((millis() - now) > 600000) { //600000 - 10 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}
}
