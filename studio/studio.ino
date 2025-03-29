//************************************************************
// read temperature and humidity "studio"
//////1 spengo studio - 2 accendo studio - 3 spengo corridoio - 4 accendo corridoio - 0 spengo tutto
// ESP32C3 Dev Module 
// 
// rele 2 studio
// rele 0 corridoio
// dht D4--> 2
// pulsante 2 studio
// pulsante 3 corridoio
// temperatura umidità AHT SDA 8 SCL 9
// 
//painlessmesh 1.5.3 arduinojson 6.21.5 esp32 3.0.7
//************************************************************
#include "Button2.h"
#include <AHTxx.h>
float h,t;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
#define BUTTON_1        2
#define BUTTON_2        3 //corridoio

int relay_studio = 1;
int relay_corridoio = 0;

bool relayState1 = LOW;
bool relayState2 = LOW;

AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2); //corridoio
//#include "painlessMesh.h"
#include "../namedmesh/namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "studio"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "studio"
#define VERSION "Studio v4.0.3"
#define MESSAGE "studio "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_AHT();
  char temp_str[10], hum_str[10];
  snprintf(temp_str, sizeof(temp_str), "%.2f", t);
  snprintf(hum_str, sizeof(hum_str), "%.2f", h);
  msg = "temperatura/";
  msg += temp_str;
  mesh.sendSingle(to, msg);
  msg = "umidita/";
  msg += hum_str;
  mesh.sendSingle(to, msg);
  taskSendMessage.setInterval(random(TASK_SECOND * 300, TASK_SECOND * 400));
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
      digitalWrite(relay_studio, LOW);
      relayState1 = LOW;
      msg = "output/1";
      mesh.sendSingle(to, msg);
  }
    else if (strcmp(rel.c_str(),"2") == 0) {
      digitalWrite(relay_studio, HIGH);
      relayState1 = HIGH;
      msg = "output/2";
      mesh.sendSingle(to, msg);
  }
  
   else if (strcmp(rel.c_str(),"3") == 0) {
    digitalWrite(relay_corridoio, LOW); 
    relayState2 = LOW;
    msg = "output/3";
    mesh.sendSingle(to, msg);
  } 
  else if (strcmp(rel.c_str(),"4") == 0) {
    digitalWrite(relay_corridoio, HIGH);
    relayState2 = HIGH;
    msg = "output/4";
    mesh.sendSingle(to, msg);
  } 
  else if (strcmp(rel.c_str(),"0") == 0) {
    digitalWrite(relay_studio, LOW);
    digitalWrite(relay_corridoio, LOW);
    relayState1 = LOW;
    relayState2 = LOW;
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
          //Serial.println("Failed to read from AHT sensor!");
          msg = "error/Failed to read from AHT sensor!";
          mesh.sendSingle(to, msg);
          return;
        }}

void button_setup() {
  btn1.setPressedHandler([](Button2 & b) {
    relayState1 = !relayState1;
    digitalWrite(relay_studio,relayState1);
    msg = "output/";
    if (relayState1 == HIGH){
      msg += "2";
    } else {
      msg += "1";
    }
    mesh.sendSingle(to, msg);
  });

  btn2.setPressedHandler([](Button2 & b) {
    relayState2 = !relayState2;
    digitalWrite(relay_corridoio,relayState2);
    msg = "output/";
    if (relayState2 == HIGH){
      msg += "4";
    } else {
      msg += "3";
    }
    mesh.sendSingle(to, msg);
  });
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
  }
}

void setup() {
  Serial.begin(115200);
  aht20.begin();
  pinMode(relay_studio, OUTPUT);
  pinMode(relay_corridoio, OUTPUT);

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

  button_setup();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  btn1.loop();
  btn2.loop();
}
