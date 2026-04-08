//************************************************************
// cantina ESP32C3 Dev Module
//BME280 temperatura umidità pressione altitudine
// ds18b20 esp32 21
// leggi acqua esp32 0
//
// esp32 2.0.17
//************************************************************
//************************************************************
// cantina ESP32C3 Dev Module - VERSIONE OTTIMIZZATA v4.0.4
//************************************************************
// Metti 'false' quando la monti definitivamente senza PC.
// Se la lasci 'true' senza USB collegata, la C3 potrebbe bloccarsi dopo un po'.
#define ENABLE_DEBUG false
// --------------------------------

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "namedMesh.h"

// Macro per il debug sicuro (non toccare)
#if ENABLE_DEBUG
  #define LOG_BEGIN(x) Serial.begin(x)
  #define LOG_PRINT(...) Serial.print(__VA_ARGS__)
  #define LOG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define LOG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG_BEGIN(x)
  #define LOG_PRINT(...)
  #define LOG_PRINTLN(...)
  #define LOG_PRINTF(...)
#endif

// Sensori
OneWire oneWire(21);
DallasTemperature sensors(&oneWire);
Adafruit_BME280 bme; 

// Pin e Variabili Acqua
const int pin_acqua = 0; // OCCHIO: Se all'avvio è a massa, entra in boot mode!
void IRAM_ATTR leggi_acqua();
volatile unsigned long lastInterruptTime = 0;
unsigned long debounceDelay = 200; 
volatile unsigned long impulsi_accumulati = 0;

// Variabili Mesh e Dati
String msg;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 min
bool retryTaskEnabled = false; 
float t,h,p,a,td;
#define SEALEVELPRESSURE_HPA (1013.25)

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; 
namedMesh  mesh;

String nodeName = "cantina"; 
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "cantina"
#define VERSION "Cantina v4.0.5"
#define MESSAGE "cantina "

// --- PROTOTIPI ---
void sendWater();    // Veloce
void sendEnv();      // Lento
void sendStatus();   // Molto Lento (Concatenato)
void retryRoot();

// --- DEFINIZIONE TASK ---
// 1. Task Acqua: parte veloce
Task taskWater( TASK_SECOND * 45, TASK_FOREVER, &sendWater );
// 2. Task Ambiente: ogni 5 minuti
Task taskEnv( TASK_MINUTE * 5, TASK_FOREVER, &sendEnv );
// 3. Task Status: ogni 10 minuti
Task taskStatus( TASK_MINUTE * 10, TASK_FOREVER, &sendStatus );
// 4. Task Retry
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

// -----------------------------------------------------------
// TASK 1: ACQUA (30-60 secondi)
// -----------------------------------------------------------
void sendWater() {
  // 1. Facciamo una copia "atomica" degli impulsi attuali
  noInterrupts();
  unsigned long da_inviare = impulsi_accumulati;
  interrupts();

  if (da_inviare == 0) {
    // Niente acqua, niente spreco di tempo.
    taskWater.setInterval(random(TASK_SECOND * 30, TASK_SECOND * 60));
    return;
  }

  // 2. Prepariamo il messaggio
  msg = "acqua/";
  msg += da_inviare;

  // 3. Proviamo l'invio e controlliamo il risultato (bool)
  // mesh.sendSingle restituisce true solo se il destinatario è in tabella e il buffer lo accetta
  if (mesh.sendSingle(to, msg)) {
    // SUCCESSO: La Mesh ha preso il pacchetto
    LOG_PRINTF(">>> [ACQUA] Inviati %lu impulsi. Reset contatore.\n", da_inviare);
    
    noInterrupts();
    // Sottraiamo solo quelli che abbiamo effettivamente inviato!
    // Se nel frattempo ne è arrivato un altro, non lo perdiamo.
    impulsi_accumulati -= da_inviare; 
    interrupts();
  } 
  else {
    // FALLIMENTO: Bridge non trovato o Mesh instabile
    LOG_PRINTF(">>> [WARNING] Bridge non raggiungibile. Mantengo %lu impulsi nel buffer...\n", impulsi_accumulati);
    
    // Non azzeriamo nulla. Al prossimo giro (tra 30-60 sec) riproveremo 
    // inviando la somma di quelli vecchi + quelli nuovi arrivati nel frattempo.
  }

  taskWater.setInterval(random(TASK_SECOND * 30, TASK_SECOND * 60));
}

// -----------------------------------------------------------
// TASK 2: AMBIENTE (5-6 minuti)
// -----------------------------------------------------------
void sendEnv() {
  // Lettura BME280
  h = bme.readHumidity();
  t = bme.readTemperature();
  p = bme.readPressure()/100;
  a = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
  if (isnan(h) || isnan(t)) {
     msg = "error/BME280_Fail";
     mesh.sendSingle(to, msg);
  } else {
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
  }

  // Lettura Dallas
  sensors.requestTemperatures(); // Blocca per ~750ms (inevitabile con libreria standard)
  td = sensors.getTempCByIndex(0);
  
  if (td == -127.0 || isnan(td)) {
      msg = "error/Dallas_Fail";
      mesh.sendSingle(to, msg);
  } else {
      msg = "temperaturad/";
      msg += td;
      mesh.sendSingle(to, msg);
  }

  // Riprogramma random tra 5 e 6 minuti
  taskEnv.setInterval( random( TASK_MINUTE * 5, TASK_MINUTE * 6 ));
}

// -----------------------------------------------------------
// TASK 3: STATUS (10 minuti - Concatenato)
// -----------------------------------------------------------
void sendStatus() {
  long uptime = millis() / 60000L;
  
  // 1. Uptime singolo (come richiesto)
  msg = "uptime/";
  msg += uptime;
  mesh.sendSingle(to, msg);

  // 2. Il resto concatenato in un'unica stringa
  // Formato: id:xxx;mem:xxx;ver:xxx;...
  String info = "cantina_info/"; // Nuovo topic unico per i dettagli
  info += "id:" + String(mesh.getNodeId()) + ";";
  info += "mem:" + String(ESP.getFreeHeap()) + ";";
  info += "ver:" + String(VERSION) + ";";
  info += "root:" + String(root_id) + ";";
  info += "ip:" + WiFi.localIP().toString() + ";";
  info += "rssi:" + String(WiFi.RSSI());
  
  mesh.sendSingle(to, info);

  // Riprogramma random tra 10 e 11 minuti
  taskStatus.setInterval( random( TASK_MINUTE * 10, TASK_MINUTE * 11 ));
}

// -----------------------------------------------------------
// CALLBACK MESH
// -----------------------------------------------------------
void receivedCallback( uint32_t from, String &msg ) {
  LOG_PRINTF("RX from %u msg=%s\n", from, msg.c_str());
  if (root_id != 0 && from != 0 && root_id != from) {
      LOG_PRINTF("!!! CRITICO: Root ID cambiato da %u a %u. Riavvio per pulizia cache.\n", root_id, from);
      delay(200); 
      ESP.restart();
  }
  root_id = from;
}

void newConnectionCallback(uint32_t nodeId) {
  LOG_PRINTF("--> New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  LOG_PRINTLN("changedConnectionCallback");
  if (!mesh.isConnected(root_id)) {
    LOG_PRINTLN("Root perso. Retry attivo.");
    if (!retryTaskEnabled) { 
      retryRootTask.enable();
      retryTaskEnabled = true; 
    }
  } else {
    LOG_PRINTLN("Root ripristinato.");
    retryRootTask.disable();
    retryTaskEnabled = false; 
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  LOG_PRINTF("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

// -----------------------------------------------------------
// INTERRUPT E SETUP
// -----------------------------------------------------------
void leggi_acqua() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) {
    impulsi_accumulati++;
  }
  lastInterruptTime = interruptTime;
}

void retryRoot() {
  if (!mesh.isConnected(root_id)) {
    LOG_PRINTLN("Ancora offline. Riavvio...");
    ESP.restart();
  } else {
    retryTaskEnabled = false; 
  }
}

void setup() {
  LOG_BEGIN(115200);
  
  bme.begin(0x77);
  sensors.begin();
  
  // INPUT_PULLUP per stabilità hardware sul Pin 0
  pinMode(pin_acqua, INPUT_PULLUP); 

  #if ENABLE_DEBUG
    mesh.setDebugMsgTypes( ERROR | STARTUP ); 
  #else
    mesh.setDebugMsgTypes( 0 ); 
  #endif

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 11 );
  mesh.initOTAReceive(ROLE);
  mesh.setContainsRoot(true);
  mesh.setName(nodeName);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Attiva i 3 Task separati
  userScheduler.addTask( taskWater );
  userScheduler.addTask( taskEnv );
  userScheduler.addTask( taskStatus );
  
  taskWater.enable();
  taskEnv.enable();
  taskStatus.enable();

  attachInterrupt(digitalPinToInterrupt(pin_acqua), leggi_acqua, FALLING);
}

void loop() {
  mesh.update();
}