//************************************************************
// read temperature and humidity "bagno"
//// ESP32-C3-MINI
// 
// temperatura umidità AHT20
// pulsante D5
// relay D4 
// painlessmesh 1.5.4 arduinojson 7.0.4
//************************************************************
#include "Button2.h"
#include <AHTxx.h>
float h,t;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
#define BUTTON_1        0
int relay = 4;
bool relayState = LOW;

Button2 btn1(BUTTON_1);
//#include "painlessMesh.h"
#include "../namedmesh/namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "bagno"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id =0;

#define ROLE    "bagno"
#define VERSION "Bagno v4.0.6"
#define MESSAGE "bagno "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_AHT();
  // Usa sprintf o snprintf per evitare la frammentazione della memoria
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  msg = "umidita/";
  msg += h;
  mesh.sendSingle(to, msg);
  
  taskSendMessage.setInterval( random( TASK_SECOND * 300, TASK_SECOND * 400 ));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 500, TASK_SECOND * 600 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel=msg;
  root_id=from;

   if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay, HIGH);   
      relayState = HIGH;
      msg = "output/1";
      mesh.sendSingle(to, msg);
   }
      else if (strcmp(rel.c_str(),"0") == 0) {
        digitalWrite(relay, LOW);   
        relayState = LOW;
        msg = "output/0";
      mesh.sendSingle(to, msg);
    }
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

void read_AHT() {
  h = aht20.readHumidity();
  t = aht20.readTemperature();
  if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from DHT sensor!");
          msg = "error/Failed to read from AHT sensor!";
          mesh.sendSingle(to, msg);
          return;
        }}

void button_setup() {
   btn1.setPressedHandler([](Button2 & b) {
      relayState = !relayState;
      digitalWrite(relay,relayState);
    if (relayState == HIGH){
      msg = "output/1";
      mesh.sendSingle(to, msg);
     }
    else if (relayState == LOW){
      msg = "output/0";
      mesh.sendSingle(to, msg);
     }
    });
}

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
  aht20.begin();
  pinMode(relay, OUTPUT);

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
  userScheduler.addTask( taskSendMessage1 );
  taskSendMessage.enable();
  taskSendMessage1.enable();

  button_setup();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  btn1.loop();
}