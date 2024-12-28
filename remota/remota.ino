//************************************************************
// remota esp32-c3 mini
// ESP32C3 Dev Module
//
// pioggia 0
//
//
//************************************************************
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
void ICACHE_RAM_ATTR leggi_pioggia();
int pioggia = 0;
volatile boolean pubblicaPioggia = false;

float t,h,p,a;
int32_t temperature;
int32_t pressure;
int16_t retT;
int16_t retP;
int16_t oversampling = 7;
String msg;
unsigned long now;
int Statoreed = 0; // variabile per la lettura del contatto
int Statoreed_old = 0; // variabile per evitare doppio conteggio
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;

String nodeName = "remota"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "remota"
#define VERSION "Remota v3.0.1"
#define MESSAGE "remota "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();


Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );

void sendMessage() {
  read_bme280();
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  msg = "umidita/";
  msg += h;
  mesh.sendSingle(to, msg);
  msg = "pressione/";
  msg += p;
  mesh.sendSingle(to, msg);
  msg = "altitudine/";
  msg += a;
  mesh.sendSingle(to, msg);
  
  taskSendMessage.setInterval( random( TASK_SECOND * 200, TASK_SECOND * 300 ));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 400, TASK_SECOND * 500 ));
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

void read_bme280() {
  h = bme.readHumidity();
  t = bme.readTemperature();
  p = bme.readPressure()/100;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from DHT sensor!");
          msg = "error/FailedtoreadfromBMEsensor";
          mesh.sendSingle(to, msg);
          return;
        }}

void leggi_pioggia()
{
  pubblicaPioggia = true;
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

void setup() {
  Serial.begin(115200);
  bme.begin(0x77);
  pinMode(pioggia, INPUT);
  
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
  userScheduler.addTask( taskSendMessage1 );
  taskSendMessage.enable();
  taskSendMessage1.enable();
  
  attachInterrupt(digitalPinToInterrupt(pioggia), leggi_pioggia, FALLING);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if (pubblicaPioggia == true) {
    msg = "pioggia/1";
    mesh.sendSingle(to, msg);
    pubblicaPioggia = false;
  }
  
    if((millis() - now) > 600000) { //600000 - 10 min
      now=millis();
      if(!(mesh.isConnected(root_id))){
        ESP.restart();
    } } }