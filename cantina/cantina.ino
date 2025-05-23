//************************************************************
// cantina ESP32C3 Dev Module
//BME280 temperatura umidità pressione altitudine
// ds18b20 su D6 esp32 8
// leggi acqua D5 esp32 0
//
// painlessmesh 1.5.3 arduinojson 7.3.0 esp32 3.0.7
//************************************************************
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
OneWire oneWire(21);
DallasTemperature sensors(&oneWire);

//void ICACHE_RAM_ATTR leggi_acqua();
void IRAM_ATTR leggi_acqua();
int acqua = 0;
volatile boolean pubblicaAcqua = false;
volatile unsigned long lastInterruptTime = 0;
unsigned long debounceDelay = 200; // 200ms debounce

String msg;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
float t,h,p,a,td;
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

#include "../namedmesh/namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
namedMesh  mesh;

String nodeName = "cantina"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "cantina"
#define VERSION "Cantina v4.0.2"
#define MESSAGE "cantina "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

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
  read_dallas();
  msg = "temperaturad/";
  msg += td;
  mesh.sendSingle(to, msg);
  taskSendMessage.setInterval( random( TASK_SECOND * 400, TASK_SECOND * 500 ));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 600, TASK_SECOND * 700 ));
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
  Serial.println("changedConnectionCallback");
  if (!mesh.isConnected(root_id)) {
    Serial.println("Connessione al root persa! Pianifico il retry.");
    if (!retryTaskEnabled) { // Controlla il flag
      retryRootTask.enable();
      retryTaskEnabled = true; // Imposta il flag
    }
  } else {
    Serial.println("Connessione al root ripristinata.");
    retryRootTask.disable();
    retryTaskEnabled = false; // Resetta il flag
}}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void leggi_acqua() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) {
    pubblicaAcqua = true;
  }
  lastInterruptTime = interruptTime;
}

void read_dallas() {
  sensors.requestTemperatures();
  td=sensors.getTempCByIndex(0);
  if (isnan(td)) {
          //Serial.println("Failed to read from Dallas sensor!");
          msg = "error/FailedtoreadfromDallasSensor";
          mesh.sendSingle(to, msg);
          return;
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

void read_bme280() {
  h = bme.readHumidity();
  t = bme.readTemperature();
  p = bme.readPressure()/100;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (isnan(h) || isnan(t)) {
          //Serial.println("Failed to read from BME280 sensor!");
          msg = "error/FailedtoreadfromBME280sensor";
          mesh.sendSingle(to, msg);
          return;
}}

void retryRoot() {
  if (!mesh.isConnected(root_id)) {
    Serial.println("Ancora offline dopo il retry. Riavvio...");
    ESP.restart();
  } else {
    Serial.println("Root tornato online.");
    retryTaskEnabled = false; // Resetta il flag anche qui, per sicurezza
}}

void setup() {
  Serial.begin(115200);
  pinMode( 21, INPUT );
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
}}