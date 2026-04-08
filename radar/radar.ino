//************************************************************
// RADAR esp32-c3 mini (Ex Remota)
// HLK-LD2410(BLE) su Serial1 (RX=20, TX=21)
//
// Sensori Legacy mantenuti (BME280, Pioggia Pin 0)
//
// esp32 2.0.17
// temp umidità AHT20
//************************************************************
// RADAR NODE - ARES OPTIMIZED v1.0
//************************************************************
#define ENABLE_DEBUG true

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <HardwareSerial.h> // Necessaria per il Radar
#include "namedMesh.h"
#include <AHTxx.h>
float h,t;
AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
// --- MACRO DEBUG ---
#if ENABLE_DEBUG
  #define LOG_BEGIN(x) Serial.begin(x)
  #define LOG_PRINTLN(x) Serial.println(x)
  #define LOG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG_BEGIN(x)
  #define LOG_PRINTLN(x)
  #define LOG_PRINTF(...)
#endif

// --- CONFIGURAZIONE RADAR ---
// ESP32-C3 SuperMini: RX=20, TX=21
#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
HardwareSerial RadarSerial(1); // Usa UART1

// Variabili Radar
int radar_presence_state = 0; // 0=Assente, 1=In Movimento, 2=Statico, 3=Entrambi
int last_sent_radar_state = -1;
unsigned long lastRadarKeepAlive = 0;

String msg; 

// --- MESH ---
#define MESH_PREFIX   "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT     5555

Scheduler userScheduler;
namedMesh mesh;

String nodeName = "radar"; 
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "radar"
#define VERSION "Radar v1.0"

// --- PROTOTIPI ---
void sendStatus();
void sendMessage() ;
void readRadarLoop(); // Funzione di lettura rapida
void manageRadarSend(); // Task di gestione invio
void retryRoot();

// --- TASKS ---
Task taskStatus(TASK_MINUTE * 10, TASK_FOREVER, &sendStatus);
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
// Radar Send: controlla spesso (es. ogni 200ms) se c'è stato un cambio di stato da inviare
Task taskRadarCheck(200, TASK_FOREVER, &manageRadarSend);
Task retryRootTask(1200000, TASK_ONCE, &retryRoot);

// -----------------------------------------------------------
// LOGICA RADAR (PARSER SERIALE)
// -----------------------------------------------------------
// Viene chiamata nel loop principale per velocità
void readRadarLoop() {
  // Il protocollo HLK invia pacchetti rapidi. Dobbiamo svuotare il buffer.
  while (RadarSerial.available() >= 4) {
    // Cerca Header: F4 F3 F2 F1
    if (RadarSerial.read() == 0xF4) {
      if (RadarSerial.read() == 0xF3) {
        if (RadarSerial.read() == 0xF2) {
          if (RadarSerial.read() == 0xF1) {
            // Header Trovato! 
            // La struttura standard Engineering è complessa, usiamo offset fissi per Basic Mode.
            // Solitamente: Len(2) + Type(1) + Head(1) + TargetState(1) ...
            
            // Leggiamo i prossimi bytes necessari
            // Byte 0-1: Data Length (Low, High)
            int lenL = RadarSerial.read();
            int lenH = RadarSerial.read();
            // Byte 2: Data Type
            int type = RadarSerial.read();
            // Byte 3: Data Head (0xAA)
            int head = RadarSerial.read();
            // Byte 4: Target State (Quello che ci serve!)
            int state = RadarSerial.read();

            // Aggiorna variabile globale se il byte è valido
            if (state >= 0 && state <= 3) {
               radar_presence_state = state;
            }

            // Svuota il resto del pacchetto per non leggere spazzatura
            // (Lettura approssimativa per liberare il buffer fino al footer F8 F7 F6 F5)
             while(RadarSerial.available() > 0 && RadarSerial.peek() != 0xF4) {
               RadarSerial.read();
             }
          }
        }
      }
    }
  }
}

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

void manageRadarSend() {
  // 1. Logica: Invio su Cambio di Stato
  // Semplifichiamo lo stato: 0 = Nessuno, >0 = Qualcuno
  int simple_state = (radar_presence_state > 0) ? 1 : 0;
  int last_simple = (last_sent_radar_state > 0) ? 1 : 0;

  bool force_send = (millis() - lastRadarKeepAlive > 60000); // Keepalive ogni 60s

  if (simple_state != last_simple || force_send) {
    
    // Preparo messaggio
    msg = "presenza/" + String(simple_state); 
    
    // Aggiungi dettaglio debug se necessario (0=Nulla, 1=Mov, 2=Stat, 3=Mix)
    // msg += ";raw:" + String(radar_presence_state);

    if (mesh.isConnected(root_id)) {
      if (mesh.sendSingle(to, msg)) {
        LOG_PRINTF(">>> [RADAR] Stato: %d (Raw: %d). Inviato.\n", simple_state, radar_presence_state);
        last_sent_radar_state = radar_presence_state;
        lastRadarKeepAlive = millis();
      }
    }
  }
}

// -----------------------------------------------------------
// TASK STATUS
// -----------------------------------------------------------
void sendStatus() {
  if (root_id == 0 || !mesh.isConnected(root_id)) return;

  mesh.sendSingle(to, "uptime/" + String(millis() / 60000L));

  msg = "radar_info/"; // Topic aggiornato
  msg += "id:" + String(mesh.getNodeId()) + ";";
  msg += "mem:" + String(ESP.getFreeHeap()) + ";";
  msg += "rssi:" + String(WiFi.RSSI()) + ";";
  msg += "ver:" + String(VERSION) + ";";
  msg += "radar_raw:" + String(radar_presence_state); // Info utile
  mesh.sendSingle(to, msg);
}

// -----------------------------------------------------------
// CALLBACK E SETUP
// -----------------------------------------------------------
void receivedCallback(uint32_t from, String &msg) {
  LOG_PRINTF("RX da %u: %s\n", from, msg.c_str());
  // --- LOGICA ARES HIGHLANDER ---
  if (root_id != 0 && from != 0 && root_id != from) {
      LOG_PRINTF("!!! CRITICO: Cambio Root ID. Riavvio.\n");
      delay(200); 
      ESP.restart();
  }
  root_id = from;
}

void changedConnectionCallback() {
  if (root_id != 0 && !mesh.isConnected(root_id)) {
    if (!retryRootTask.isEnabled()) retryRootTask.enable();
  }
}

void retryRoot() {
  if (!mesh.isConnected(root_id)) ESP.restart();
}

void read_AHT() {
  h = aht20.readHumidity();
  t = aht20.readTemperature();
  if (isnan(h) || isnan(t)) {
          msg = "error/Failed to read from AHT sensor!";
          mesh.sendSingle(to, msg);
          return;
}}

void setup() {
  LOG_BEGIN(115200);
  aht20.begin();
  // --- INIT RADAR SERIAL ---
  // Importante: HLK-LD2410 default è 256000 baud
  RadarSerial.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  LOG_PRINTLN("Radar Serial Init...");

  // Mesh Init
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  // NOTA: Canale 11 come richiesto, ma ricorda il vincolo router
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 11);
  mesh.initOTAReceive(ROLE);
  mesh.setName(nodeName);
  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  userScheduler.addTask(taskStatus);
  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask(taskRadarCheck); // Nuovo task radar
  
  taskStatus.enable();
  taskSendMessage.enable();
  taskRadarCheck.enable();
}

void loop() {
  mesh.update();
  
  // Lettura radar nel loop principale per non perdere pacchetti seriali
  readRadarLoop();
}