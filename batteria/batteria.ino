//************************************************************
// batteria con piccoli pannelli fotovoltaici wemos d1 mini clone
//
// volt batteria A0
//
//
//************************************************************

#include <MicroWakeupper.h>
float volt;
String msg;
unsigned long now;

//MicroWakeupper instance (only one is supported!)
MicroWakeupper microWakeupper;

//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;

String nodeName = "batteria"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "batteria"
#define VERSION "Batteria v1.0.0"
#define MESSAGE "batteria "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain


Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {

  //msg = "volt/";
  //msg += microWakeupper.readVBatt();
  //mesh.sendSingle(to, msg);
  battery();
  update_status();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 100, TASK_SECOND * 200 ));

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  root_id=from;
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
  msg = "wifisignal/";
  msg += String(WiFi.RSSI());
  mesh.sendSingle(to, msg);
  
}

void battery(){
  msg = "volt/";
  msg += microWakeupper.readVBatt();
  mesh.sendSingle(to, msg);
}
void setup() {
  Serial.begin(115200);
  microWakeupper.begin();
  
  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

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
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
 // if(mesh.isConnected(root_id)){
  //      Serial.printf("Dormi\n");
  //      //ESP.restart();
  //      ESP.deepSleep(90 * 1000000);
        //ESP.deepSleep(ESP.deepSleepMax());
  //  }
    if((millis() - now) > 300000) { //600000 - 10 min
      Serial.printf("millis\n");
      Serial.println(root_id);
      now=millis();
     
      //if(!(mesh.isConnected(root_id))){
      if(mesh.isConnected(root_id)){
        Serial.printf("Reboot\n");
        //ESP.restart();
        //ESP.deepSleep(90 * 1000000);
        ESP.deepSleep(ESP.deepSleepMax());
    } } }
