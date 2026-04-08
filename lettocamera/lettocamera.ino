//************************************************************
// lettura presenza nel letto con HX711
// wemos d1 mini 
// 
// esp8266 2.0.17
// esp32-s3 11-12
//
//************************************************************
#include <HX711.h>
// HX711 Pins
const int LOADCELL_DOUT_PIN = 11; //D5; // D4 Remember these are ESP GPIO pins, they are not the physical pins on the board.
const int LOADCELL_SCK_PIN = 12; //D6; //RX
int calibration_factor = 400; // Defines calibration factor we'll use for calibrating. se diminuisce aumenta il valore inversamente proporzionale
float reading; // Float for reading
float raw; // Float for raw value which can be useful
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
// Variabili e costanti per la gestione errori HX711 e riavvio
int hx711_error_count = 0; // Contatore errori consecutivi HX711
const int MAX_HX711_ERRORS = 5; // Numero massimo di errori HX711 prima del riavvio

HX711 scale;                          // Initiate HX711 library

//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
//String msg,rel;
String nodeName = "lettocamera"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "lettocamera"
#define VERSION "LettoCamera v3.1.2"
#define MESSAGE "lettocamera "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_peso();
  taskSendMessage.setInterval( random( TASK_SECOND * 300, TASK_SECOND * 400 ));
  //taskSendMessage.setInterval( TASK_SECOND * 5 );
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 600, TASK_SECOND * 700 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  String rel = msg;
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if (root_id != 0 && from != 0 && root_id != from) {
      delay(200); 
      ESP.restart();
  }
  root_id=from;
  rel=msg;

    if (strcmp(rel.c_str(),"1") == 0){
      //tara la bilancia
      //scale.power_up();
      //scale.wait_ready();
      if (scale.is_ready())
      {
      scale.set_scale(calibration_factor);
      scale.tare();       //Reset scale to zero
      //scale.power_down();
  }}}

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

void read_peso() {
  String msg;
  //Serial.println("-> Accensione HX711 (power_up)");
  //scale.power_up(); 
  
  // Verifica se la bilancia risponde
  bool ready = scale.is_ready();
  //bool ready = scale.wait_ready_timeout(50);
  Serial.print("-> Stato HX711 (is_ready): ");
  if(ready) Serial.println("PRONTO (OK)");
  else Serial.println("NON PRONTO (ERRORE)");

  if (ready) {
    hx711_error_count = 0;
    scale.set_scale(calibration_factor); 
    
    // Lettura
    reading = scale.get_units(10); 
    raw = scale.read_average(5); 

    if (reading < 0) reading = 0.00; 

    // *** STAMPA I VALORI SUL MONITOR ***
    Serial.print("-> VALORI LETTI: Raw=");
    Serial.print(raw);
    Serial.print(" | Peso=");
    Serial.println(reading);

    char reading_str[20];
    char raw_str[20];
    snprintf(reading_str, sizeof(reading_str), "%.2f", reading); 
    snprintf(raw_str, sizeof(raw_str), "%.0f", raw); 

    msg = "peso/";
    msg += reading_str;
    mesh.sendSingle(to, msg);

    msg = "valuerawstr/";
    msg += raw_str;
    mesh.sendSingle(to, msg);
    Serial.println("-> Dati inviati alla Mesh");
    
  } else {
    hx711_error_count++;
    Serial.print("-> ERRORE CRITICO: HX711 muto. Contatore errori: ");
    Serial.println(hx711_error_count);
    
    msg = "error/HX711notready";
    mesh.sendSingle(to, msg);
    
    if (hx711_error_count >= MAX_HX711_ERRORS) { // Aumentato tolleranza per debug
      Serial.println("-> Troppi errori consecutivi. Riavvio ESP...");
      delay(1000); 
      ESP.restart(); 
    }
  }
  //scale.power_down(); 
  //Serial.println("-> HX711 spento (power_down)");
}

void update_status() {
  String msg; // Variabile locale
  
  // 1. LEGGIAMO I DATI PRIMA DI TUTTO (Così esistono per tutta la funzione)
  long uptime = millis() / 60000L;
  long freeMem = ESP.getFreeHeap(); // <--- ORA ESISTE
  long rssi = WiFi.RSSI();          // <--- ORA ESISTE

  // 2. Invio UPTIME (Separato per OpenHAB)
  msg = "uptime/";
  msg += uptime;
  mesh.sendSingle(to, msg);
  
  // 3. Invio DEVICE STATUS (Pacchetto Unico)
  msg = "devicestatus/"; 
  msg += String(freeMem) + ";";       // Usiamo la variabile letta sopra
  msg += String(rssi) + ";";          // Usiamo la variabile letta sopra
  msg += WiFi.localIP().toString() + ";"; 
  msg += String(mesh.getNodeId()) + ";"; 
  msg += VERSION; 

  mesh.sendSingle(to, msg);

  // 4. STAMPA DI DIAGNOSTICA SUL SERIALE (Ora funziona perché le variabili esistono)
  Serial.println("--- STATUS REPORT ---");
  Serial.printf("-> Uptime: %ld min\n", uptime);
  Serial.printf("-> Free RAM: %ld bytes\n", freeMem); 
  Serial.printf("-> WiFi Signal: %ld dBm\n", rssi);
  Serial.println("---------------------");
  
  Serial.println("-> Status inviato (Uptime + DeviceStatus)");
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
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);   // Start scale on specified pins
  scale.power_up();
  //scale.wait_ready();                                 //Ensure scale is ready, this is a blocking function
  scale.set_scale(calibration_factor);                                  
  //scale.wait_ready();
  //scale.tare();                                       // Tare scale on startup
  //scale.wait_ready();

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
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  // Se sono passate 2 ore (7200000 ms), riavvia per pulire tutto
 // if (millis() > 7200000) {
 //    Serial.println("Riavvio programmato manutenzione RAM...");
 //    delay(1000);
 //    ESP.restart();
//  }
}