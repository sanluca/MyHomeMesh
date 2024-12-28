//************************************************************
//
// Read Consuption with Pzem-004
// wemos d1 mini pro
//
//************************************************************
float voltg,curg,powerg,energ,freqg,pfg,volta,cura,powera,enera,freqa,pfa,voltc,curc,powerc,enerc,freqc,pfc;

unsigned long now;

#include <PZEM004Tv30.h>
//PZEM004Tv30 pzem(&Serial3);
PZEM004Tv30 pzem (D5, D6);
PZEM004Tv30 pzem2 (D3, D4);
PZEM004Tv30 pzem3 (D7, D0);
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "enel"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "enel"
#define VERSION "Enel v3.1.0"
#define MESSAGE "enel "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void sendMessage2();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task taskSendMessage2( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage2 );

void sendMessage() {
  //generale
  read_enel_gen();
  taskSendMessage.setInterval( random( TASK_SECOND * 3, TASK_SECOND * 6 ));
}
void sendMessage1() {
//appartamento
  read_enel_ap();
//cantina garage
  read_enel_cant();
  taskSendMessage1.setInterval( random( TASK_SECOND * 8, TASK_SECOND * 12 ));
}

void sendMessage2() {
//update status
update_status();
  taskSendMessage2.setInterval( random( TASK_SECOND * 450, TASK_SECOND * 550 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel=msg;
  root_id=from;
  
  if (strcmp(rel.c_str(),"1") == 0){
    pzem.resetEnergy();
    pzem2.resetEnergy();
    pzem3.resetEnergy();
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

void read_enel_gen() {
  voltg = pzem.voltage();
  curg = pzem.current();
  powerg = pzem.power();
  energ = pzem.energy();
  freqg = pzem.frequency();
  pfg = pzem.pf();
  if (!(isnan(voltg))){
  msg = "voltg/";
  msg += voltg;
  mesh.sendSingle(to, msg);
  msg = "curg/";
  msg += curg;
  mesh.sendSingle(to, msg);
  msg = "powerg/";
  msg += powerg;
  mesh.sendSingle(to, msg);
  msg = "energ/";
  msg += energ;
  mesh.sendSingle(to, msg);
  msg = "freqg/";
  msg += freqg;
  mesh.sendSingle(to, msg);
  msg = "pfg/";
  msg += pfg;
  mesh.sendSingle(to, msg);
}}

void read_enel_ap() {
  volta = pzem2.voltage();
  cura = pzem2.current();
  powera = pzem2.power();
  enera = pzem2.energy();
  freqa = pzem2.frequency();
  pfa = pzem2.pf();
  if (!(isnan(volta))){
  msg = "volta/";
  msg += volta;
  mesh.sendSingle(to, msg);
  msg = "cura/";
  msg += cura;
  mesh.sendSingle(to, msg);
  msg = "powera/";
  msg += powera;
  mesh.sendSingle(to, msg);
  msg = "enera/";
  msg += enera;
  mesh.sendSingle(to, msg);
  msg = "freqa/";
  msg += freqa;
  mesh.sendSingle(to, msg);
  msg = "pfa/";
  msg += pfa;
  mesh.sendSingle(to, msg);

}}

void read_enel_cant() {
  voltc = pzem3.voltage();
  curc = pzem3.current();
  powerc = pzem3.power();
  enerc = pzem3.energy();
  freqc = pzem3.frequency();
  pfc = pzem3.pf();
  if (!(isnan(voltc))){
  msg = "voltc/";
  msg += voltc;
  mesh.sendSingle(to, msg);
  msg = "curc/";
  msg += curc;
  mesh.sendSingle(to, msg);
  msg = "powerc/";
  msg += powerc;
  mesh.sendSingle(to, msg);
  msg = "enerc/";
  msg += enerc;
  mesh.sendSingle(to, msg);
  msg = "freqc/";
  msg += freqc;
  mesh.sendSingle(to, msg);
  msg = "pfc/";
  msg += pfc;
  mesh.sendSingle(to, msg);
}}

void update_status() {
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

void setup() {
  Serial.begin(115200);

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
  userScheduler.addTask( taskSendMessage1 );
  userScheduler.addTask( taskSendMessage2 );
  taskSendMessage.enable();
  taskSendMessage1.enable();
  taskSendMessage2.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
 if((millis() - now) > 600000) { //600000 - 10 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}}