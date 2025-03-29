//************************************************************
// remota esp32-c3 mini
// ESP32C3 Dev Module
//
// pioggia 0
//
// painlessmesh 1.5.3 arduinojson 6.21.5 esp32 3.0.7
//************************************************************
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
void ICACHE_RAM_ATTR leggi_pioggia();
int pioggia = 0;
volatile boolean pubblicaPioggia = false;
volatile unsigned long lastRainInterruptTime = 0;
unsigned long rainDebounceDelay = 500; // 200ms debounce
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
float t,h,p,a;
String msg;
//#include "painlessMesh.h"
#include "../namedmesh/namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;

String nodeName = "remota"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "remota"
#define VERSION "Remota v3.2.0"
#define MESSAGE "remota "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_bme280();

  char temp_str[10], hum_str[10], press_str[10], alt_str[10];
  snprintf(temp_str, sizeof(temp_str), "%.2f", t);
  snprintf(hum_str, sizeof(hum_str), "%.2f", h);
  snprintf(press_str, sizeof(press_str), "%.2f", p);
  snprintf(alt_str, sizeof(alt_str), "%.2f", a);

  msg = "temperatura/";
  msg += temp_str;
  mesh.sendSingle(to, msg);

  msg = "umidita/";
  msg += hum_str;
  mesh.sendSingle(to, msg);

  msg = "pressione/";
  msg += press_str;
  mesh.sendSingle(to, msg);

  msg = "altitudine/";
  msg += alt_str;
  mesh.sendSingle(to, msg);

  taskSendMessage.setInterval(random(TASK_SECOND * 300, TASK_SECOND * 400));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 500, TASK_SECOND * 600 ));
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

void read_bme280() {
  h = bme.readHumidity();
  t = bme.readTemperature();
  p = bme.readPressure()/100;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (isnan(h) || isnan(t)) {
          //Serial.println("Failed to read from DHT sensor!");
          msg = "error/FailedtoreadfromBMEsensor";
          mesh.sendSingle(to, msg);
          return;
}}

void leggi_pioggia() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastRainInterruptTime > rainDebounceDelay) {
    pubblicaPioggia = true;
    lastRainInterruptTime = interruptTime;
  }
}

void update_status() {
  long uptime = millis() / 60000L;
  char uptime_str[10], nodeid_str[10], freemem_str[10], rssi_str[10];
  snprintf(uptime_str, sizeof(uptime_str), "%ld", uptime);
  snprintf(nodeid_str, sizeof(nodeid_str), "%u", mesh.getNodeId());
  snprintf(freemem_str, sizeof(freemem_str), "%u", ESP.getFreeHeap());
  snprintf(rssi_str, sizeof(rssi_str), "%d", WiFi.RSSI());

  msg = "uptime/";
  msg += uptime_str;
  mesh.sendSingle(to, msg);

  msg = "nodeid/";
  msg += nodeid_str;
  mesh.sendSingle(to, msg);

  msg = "freememory/";
  msg += freemem_str;
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
  msg += rssi_str;
  mesh.sendSingle(to, msg);
}

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
  bme.begin(0x77);
  pinMode(pioggia, INPUT);
  
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
  
  attachInterrupt(digitalPinToInterrupt(pioggia), leggi_pioggia, FALLING);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if (pubblicaPioggia == true) {
    msg = "pioggia/1";
    mesh.sendSingle(to, msg);
    pubblicaPioggia = false;
}}