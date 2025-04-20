//************************************************************
//DOIT ESP32 DEVKIT V1
//painlessmesh 1.5.3 arduinojson 6.21.5
#include <Arduino.h>
//#include <painlessMesh.h>
#include "../namedmesh/namedMesh.h"
#include <PubSubClient.h>
#include <WiFiClient.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "mesh"
#define   STATION_PASSWORD "mesh7mesh7"

#define HOSTNAME "MQTT_Bridge"

Scheduler userScheduler; // to control your personal task

#define ROLE    "mqttbridge"
#define VERSION "MQTTBRIDGE v5.0.2"
#define MESSAGE "mqttbridge "

// Prototypes
void receivedCallback( const String &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);
IPAddress mqttBroker(10, 42, 0, 1);

//painlessMesh  mesh;
namedMesh  mesh;

String msgg,meshTopology;
String nodeName = "bridgemqtt"; // Name needs to be unique

WiFiClient wifiClient;
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );

void sendMessage() {
  send_id();
  taskSendMessage.setInterval( random( TASK_SECOND * 60, TASK_SECOND * 80 ));
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 450, TASK_SECOND * 550 ));
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true); 
  //mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | DEBUG | MESH_STATUS | SYNC | COMMUNICATION | GENERAL | REMOTE | MSG_TYPES );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP ); 
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 11 );
  mesh.initOTAReceive(ROLE);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  mesh.setName(nodeName);
  mesh.onReceive(&receivedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskSendMessage1 );
  taskSendMessage.enable();
  taskSendMessage1.enable();
}

void update_status() {
  long uptime = millis() / 60000L;
  mqttClient.publish("home/bridge/uptime", String(uptime).c_str());
  mqttClient.publish("home/bridge/nodeid", String(mesh.getNodeId()).c_str());
  mqttClient.publish("home/bridge/freememory", String(ESP.getFreeHeap()).c_str());
  mqttClient.publish("home/bridge/version", VERSION);
  mqttClient.publish("home/bridge/ip", myIP.toString().c_str());

  meshTopology = mesh.subConnectionJson();
  if (!meshTopology.isEmpty()) {
    mqttClient.publish("home/bridge/topologia", meshTopology.c_str());
  }
  mqttClient.publish("home/bridge/wifisignal", String(WiFi.RSSI()).c_str());
}

void send_id()
{
  msgg = "sendrootid";
  mesh.sendBroadcast(msgg);
}

void reconnect() {
  unsigned long startAttemptTime = millis();

  while (!mqttClient.connected()) {
    mqttClient.setBufferSize(2048);
    mqttClient.setKeepAlive(90);

    if (mqttClient.connect("BridgeMqttMesh")) {
      Serial.println("Connected");
      mqttClient.publish("home/bridge/start", "Ready!");
      mqttClient.subscribe("home/comand/#");
    } else {
      if (millis() - startAttemptTime > 5000) {
        // Se la connessione fallisce per più di 5 secondi, esci dal loop
        break;
      }
      delay(500);
      mesh.update();
      mqttClient.loop();
    }
  }
}

void loop() {
  // Esegui i task pianificati dall'utente
  userScheduler.execute();
  mesh.update();
  mqttClient.loop();

   if(myIP != getlocalIP()){
    myIP = getlocalIP();
  //  Serial.println("My IP is " + myIP.toString());
}

    if (!mqttClient.connected())  {
      {reconnect();}
    }
}

void receivedCallback(const String& from, const String& msg) {
  int pos = msg.indexOf('/'); // Trova il primo '/'
  if (pos != -1) {
    String topicPart = msg.substring(0, pos); // Parte prima del '/'
    String payload = msg.substring(pos + 1); // Parte dopo il '/'

    // Pubblica il messaggio solo se il payload non è vuoto
    if (!payload.isEmpty()) {
      String mqttTopic = "home/" + from + "/" + topicPart;
      mqttClient.publish(mqttTopic.c_str(), payload.c_str());
    }
  }
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  String msg = String((char*)payload).substring(0, length);
  String targetStr = String(topic).substring(12); // home/comand/
  mesh.sendSingle(targetStr, msg);
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}