//************************************************************
// skylamp
//
// 
//
//
//************************************************************
#include <Adafruit_NeoPixel.h>
String msg;
unsigned long now;
int count=0;

#define PIN D2
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;

String nodeName = "skylamp"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "skylamp"
#define VERSION "Skylamp v1.0.3"
#define MESSAGE "skylamp "



// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain


Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {

  colors();
  Serial.printf("colors\n");
  update_status();
  Serial.printf("status\n");
  taskSendMessage.setInterval( random( TASK_SECOND * 20, TASK_SECOND * 50 ));

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("skylamp: Received from %u msg=%s\n", from, msg.c_str());
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
  Serial.printf("fine update\n");
}

void colors() {
// for(int i=0;i<NUMPIXELS;i++){

 // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
//I used the random number generator with input random(min,max) 
//to select random RGB values between 0-255
    pixels.setPixelColor(0, pixels.Color(random(0,255),random(0,255),random(0,255))); 

    pixels.show(); // This sends the updated pixel color to the hardware.
//    break;
   // delay(random(100,1000)); // Delay for some random period of time (in milliseconds), between 100 milliseconds and 1 second.

  }
  //} 

void start_init(){
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA );
  //mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA );
  mesh.initOTAReceive(ROLE);
  mesh.onReceive(&receivedCallback);
  mesh.setContainsRoot(true);
  mesh.setName(nodeName);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | APPLICATION); // all types on
  //mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  start_init();
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable(); 
}




void loop() {
  // it will run the user scheduler as well
 
 // colors();
  if((millis() - now) > 200000) { //600000 - 5 min
    now=millis();
   // if(!(mesh.isConnected(root_id))){
        mesh.stop();
        mesh.update();
        //delay(1000);
        root_id=0;
        start_init();
        mesh.update();
        mesh.nameBroadCastTask.disable();
        //mesh.nameBroadCastInit=false;
        mesh.nameBroadCastTask.forceNextIteration();
        mesh.update();
        Serial.printf("Reboot mesh\n");
    }//}
    mesh.update();
}
