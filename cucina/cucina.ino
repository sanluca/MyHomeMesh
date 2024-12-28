//************************************************************
// read temperature and humidity "cucina"
//ESP32C3 Dev Module 
////1 spengo cucina - 2 accendo cucina - 3 spengo ingresso - 4 accendo ingresso - 5 aspiratore acceso - 6 aspiratore spento - 0 spengo tutto luce
// rele D3 --> 0 cucina
// rele D4 --> 1 ingresso
// 
// pulsante D5-->2 cucina
// pulsante D6-->3 ingresso
// 
//painlessmesh 1.5.4 arduinojson 7.0.4
//************************************************************
#include "Button2.h"
#include <AHTxx.h>
float h,t;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
#define BUTTON_1        2
#define BUTTON_2        3

int relay_ingresso = 1;
int relay_cucina = 0;
bool relayState = LOW;
bool relayState1 = LOW;

AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "cucina"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "cucina"
#define VERSION "Cucina v4.0.1"
#define MESSAGE "cucina "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_AHT();
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
  taskSendMessage1.setInterval( random( TASK_SECOND * 450, TASK_SECOND * 550 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel=msg;
  root_id=from;

   if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay_cucina, LOW);
      relayState = LOW;
      msg = "output/";
      msg += "1";
      mesh.sendSingle(to, msg);
   }
      else if (strcmp(rel.c_str(),"2") == 0) {
        digitalWrite(relay_cucina, HIGH);
        relayState = HIGH;
        msg = "output/";
      msg += "2";
      mesh.sendSingle(to, msg);
    }
  
   else if (strcmp(rel.c_str(),"3") == 0) {
    digitalWrite(relay_ingresso, LOW);
    relayState1 = LOW;
    msg = "output/";
      msg += "3";
      mesh.sendSingle(to, msg);
  } 
  else if (strcmp(rel.c_str(),"4") == 0) {
    digitalWrite(relay_ingresso, HIGH);
    relayState1 = HIGH;
    msg = "output/";
      msg += "4";
      mesh.sendSingle(to, msg);
  } 

  else if (strcmp(rel.c_str(),"0") == 0) {
    digitalWrite(relay_cucina, LOW);
    digitalWrite(relay_ingresso, LOW);
    relayState1 = LOW;
    relayState = LOW;
    msg = "output/";
    msg += "0";
    mesh.sendSingle(to, msg);
  }}

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
          msg = "error/Failed to read from AHT sensor!";
          mesh.sendSingle(to, msg);
          return;
        }}

void button_setup() {
  btn1.setPressedHandler([](Button2 & b) 
    {
    relayState = !relayState;
     digitalWrite(relay_cucina,relayState);
     if (relayState == HIGH){
      msg = "output/2";
      mesh.sendSingle(to, msg);
     }
     else if (relayState == LOW){
      msg = "output/1";
      mesh.sendSingle(to, msg);
     }
    });

  btn2.setPressedHandler([](Button2 & b) {
    relayState1 = !relayState1;
     digitalWrite(relay_ingresso,relayState1);
     if (relayState1 == HIGH){
      msg = "output/4";
      mesh.sendSingle(to, msg);
     }
     else if (relayState1 == LOW){
      msg = "output/3";
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
  }
}

void setup() {
  Serial.begin(115200);
  aht20.begin();
  pinMode(relay_ingresso, OUTPUT);
  pinMode(relay_cucina, OUTPUT);

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes( ERROR | STARTUP | DEBUG );  // set before init() so that you can see startup messages
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

  button_setup();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  btn1.loop();
  btn2.loop();
}