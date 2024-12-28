//************************************************************
// cantina wemos d1 mini pro
//BME280 temperatura umidità pressione altitudine
// ds18b20 su D6
// leggi acqua D5
//
//************************************************************
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void ICACHE_RAM_ATTR leggi_acqua();
int acqua = D5;
volatile boolean pubblicaAcqua = false;

String msg;
unsigned long now;
unsigned long noww;

float t,h,p,a,td;
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
namedMesh  mesh;

String nodeName = "cantina"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "cantina"
#define VERSION "Cantina v3.0.2"
#define MESSAGE "cantina "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );

void sendMessage() {
  read_bme280();
  read_dallas();
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  msg = "pressione/";
  msg += p;
  mesh.sendSingle(to, msg);
  msg = "umidita/";
  msg += h;
  mesh.sendSingle(to, msg);
  msg = "temperaturad/";
  msg += td;
  mesh.sendSingle(to, msg);
  msg = "altitudine/";
  msg += a;
  mesh.sendSingle(to, msg);
 
  taskSendMessage.setInterval( random( TASK_SECOND * 300, TASK_SECOND * 400 ));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 450, TASK_SECOND * 550 ));
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

void leggi_acqua()
{
  pubblicaAcqua = true;
}

void read_dallas(){
  sensors.requestTemperatures();
  td=sensors.getTempCByIndex(0);
  if (isnan(td)) {
          Serial.println("Failed to read from Dallas sensor!");
          msg = "error/FailedtoreadfromDallasSensor";
          mesh.sendSingle(to, msg);
          return;
        }}

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

void read_bme280() {

  h = bme.readHumidity();
  t = bme.readTemperature();
  p = bme.readPressure()/100;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from BME280 sensor!");
          msg = "error/FailedtoreadfromBME280sensor";
          mesh.sendSingle(to, msg);
          return;
        }}

void setup() {
  Serial.begin(115200);
  bme.begin(0x77);
  sensors.begin();
  pinMode(acqua, INPUT);

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
  taskSendMessage.enable();
  taskSendMessage1.enable();

  attachInterrupt(digitalPinToInterrupt(acqua), leggi_acqua, FALLING);
}


void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if (pubblicaAcqua == true) {
    msg = "acqua/1";
    mesh.sendSingle(to, msg);
    pubblicaAcqua = false;
  }
  
    if((millis() - now) > 600000) { //600000 - 2,5 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.println("restart");
        ESP.restart();
    }}}