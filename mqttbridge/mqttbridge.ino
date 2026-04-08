//************************************************************
//ESP32 dev Module
//
#include <Arduino.h>
//#include <painlessMesh.h>
#include "namedMesh.h"
#include <PubSubClient.h>
#include <WiFiClient.h>

#define   MESH_PREFIX     "whateverYouLike"

#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "mesh18"
#define   STATION_PASSWORD "mesh18mesh18"

#define HOSTNAME "MQTT_Bridge"

Scheduler userScheduler; // to control your personal task

#define ROLE    "mqttbridge"
#define VERSION "MQTTBRIDGE v5.0.5"
#define MESSAGE "mqttbridge "

// Prototypes
void receivedCallback( const String &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);
IPAddress mqttBroker(192, 168, 1, 26);

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
  //taskSendMessage1.enable();
  delay(2000);
}


void update_status() {
  // NUOVA PROTEZIONE: Se non abbiamo IP o MQTT non è connesso, ESCE SUBITO.
  // Usiamo getlocalIP() che è sicuro perché legge una variabile interna della mesh
  if (getlocalIP() == IPAddress(0,0,0,0) || !mqttClient.connected()) return;

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

void loop() {
  mesh.update(); // Priorità alla mesh

  IPAddress localIP = getlocalIP();

  // Se abbiamo un IP valido (siamo connessi al router)
  if (localIP != IPAddress(0, 0, 0, 0)) {
    
    // Se il task MQTT è spento, ORA lo accendiamo
    if (!taskSendMessage1.isEnabled()) {
      taskSendMessage1.enable();
    }

    // Gestione MQTT
    mqttClient.loop();
    if (!mqttClient.connected()) {
      reconnect();
    }

  } else {
    // Se perdiamo l'IP, SPEGNIAMO subito il task per evitare crash
    if (taskSendMessage1.isEnabled()) {
      taskSendMessage1.disable();
    }
  }

  userScheduler.execute();
  
  // Aggiornamento variabile IP per debug
  if(myIP != localIP) {
    myIP = localIP;
  }
}

unsigned long lastReconnectAttempt = 0;

void reconnect() {
  if (getlocalIP() == IPAddress(0,0,0,0)) return;
  // Esegui il tentativo solo ogni 5 secondi
  if (millis() - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = millis();
    
    Serial.print("Tentativo connessione MQTT... ");
    
    // Aumenta buffer e timeout per stabilità
    mqttClient.setBufferSize(1024); // 2048 a volte è troppo per la heap residua con la mesh
    mqttClient.setKeepAlive(90);

    // Tenta la connessione
    if (mqttClient.connect("BridgeMqttMesh")) {
      Serial.println("Connesso!");
      mqttClient.publish("home/bridge/start", "Ready!");
      mqttClient.subscribe("home/comand/#");
      lastReconnectAttempt = 0;
    } else {
      Serial.print("fallito, rc=");
      Serial.println(mqttClient.state());
      // Non bloccare o usare delay(), lascia che il loop continui
    }
  }
}

void receivedCallback(const String& from, const String& msg) {
  //Serial.printf("MESH RX -> Da: %s | Msg: %s\n", from.c_str(), msg.c_str());
  if (!mqttClient.connected()) return;
  int pos = msg.indexOf('/'); // Trova il primo '/'
  if (pos != -1) {
    String topicPart = msg.substring(0, pos); // Parte prima del '/'
    String payload = msg.substring(pos + 1); // Parte dopo il '/'

    // Pubblica il messaggio solo se il payload non è vuoto
    if (!payload.isEmpty()) {
      String mqttTopic = "home/" + from + "/" + topicPart;
      //Serial.printf("MQTT TX -> Topic: %s | Payload: %s\n", mqttTopic.c_str(), payload.c_str());
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