//************************************************************
// Nodo Mesh: frigorifero (Hardware: ESP32-C3 Mini)
//
// Pin Mapping:
// Dallas DS18B20 -> GPIO 21
// Relè Frigo      -> GPIO 4
// Conta bolle     -> GPIO 1
// I2C (Daisy24)   -> Standard ESP32-C3 (SDA: 8, SCL: 9)
//Top-Left (1110 -> Binario 0b00001110): Spegnimento LCD
//Top-Right (1101 -> Binario 0b00001101): Termostato +
//Bottom-Right (1011 -> Binario 0b00001011): Termostato -
//Bottom-Left (0111 -> Binario 0b00000111): Sonda Libera
//************************************************************
#include <EEPROM.h>
#include <Wire.h>
#include "Daisy24.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "namedMesh.h"

#define ONE_WIRE_BUS 21
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define I2C_SDA 8
#define I2C_SCL 9

void read_bol();

float t;
int termostato;
unsigned long m;
int count = 0;
const byte relay = 4;
const byte bolle = 1;
String rel;
String rel2;
byte i;
#define pcf_add 0x3F

volatile boolean sendBolle = false;
volatile boolean relayState = LOW;

// Variabili per gestione Task Retry Riavvio
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false;

// Variabili per l'Ottimizzazione del Salvataggio (Delayed Save)
unsigned long lastSetPointTime = 0;
bool pendingSave = false;
const unsigned long SAVE_DELAY = 5000; // 5 secondi di ritardo

LCD_Daisy24 lcd;

// Variabili per Task Pulsanti Daisy24
byte statoPrecedenteTasti = 0x0F; // Riposo è 1111 (15 in decimale)
unsigned long debounceTimer = 0;
bool isLcdOn = true; // Macchina a stati per lo schermo

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; 
namedMesh mesh;

String msg;
String nodeName = "frigorifero"; 
String to = "bridgemqtt";
uint32_t root_id = 0;

#define ROLE    "frigorifero"
#define VERSION "Frigorifero v3.0.1"
#define MESSAGE "frigorifero "

// Prototypes
void sendMessage(); 
void retryRoot();
void readButtons();
void update_temperature();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot); // Task iniettato
// Interroga l'I2C ogni 100ms (10Hz). Invisibile per la CPU, perfetto per le dita umane.
Task taskReadButtons( TASK_MILLISECOND * 100, TASK_FOREVER, &readButtons );
// INIEZIONE: Nuovo task termodinamico indipendente (gira ogni 5 secondi)
Task taskControlloTermico( TASK_SECOND * 5, TASK_FOREVER, &update_temperature );

void sendMessage() {
  //update_temperature();
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  //data_lcd();
  update_status();
  turn_off_lcd();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 100, TASK_SECOND * 150 ));
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("[DEBUG MESH] Ricevuto da %u msg=%s\n", from, msg.c_str());
  root_id = from; 
  
  int nuovo_termostato = msg.toInt();
  
  if(nuovo_termostato > 1){
    if(nuovo_termostato != termostato) {
      termostato = nuovo_termostato; // Aggiornamento immediato in RAM
      
      // Innesco il timer per il salvataggio ritardato
      pendingSave = true;
      lastSetPointTime = millis();
      Serial.printf("[DEBUG RAM] Target impostato a %d C. Attesa 20s per scrittura nel silicio...\n", termostato);
    } else {
      Serial.printf("[DEBUG RAM] Target identico (%d). Nessuna azione.\n", termostato);
    }
    
    // Isolo la stringa di risposta per non corrompere il buffer
    String replyMsg = "termostato/";
    replyMsg += termostato;
    mesh.sendSingle(to, replyMsg); // Il bridge riceve conferma istantanea
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("[DEBUG MESH] Nuova connessione, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.println("[DEBUG MESH] Topologia di rete modificata");
  
  // Faglia sistemica corretta: Non scatta se root_id è 0
  if (root_id != 0 && !mesh.isConnected(root_id)) {
    Serial.println("[ERROR] Connessione al root persa! Pianifico il retry di 20 min.");
    if (!retryTaskEnabled) { 
      retryRootTask.enable();
      retryTaskEnabled = true; 
    }
  } else {
    Serial.println("[DEBUG MESH] Connessione al root ripristinata o non ancora acquisita.");
    retryRootTask.disable();
    retryTaskEnabled = false; 
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("[DEBUG MESH] Tempo allineato %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

// Funzione di Esecuzione Riavvio Iniettata
void retryRoot() {
  if (!mesh.isConnected(root_id)) {
    Serial.println("[FATAL] Ancora offline dopo il retry. Riavvio hardware forzato...");
    ESP.restart();
  } else {
    Serial.println("[DEBUG MESH] Root tornato online. Reboot annullato.");
    retryTaskEnabled = false; 
  }
}

void read_dallas(){
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  if (isnan(t) || t == -127.00) { 
    Serial.println("[ERROR SENSOR] Fallita lettura sonda Dallas!");
    msg = "error/";
    msg += "Failed to read from Dallas sensor!";
    mesh.sendSingle(to, msg);
  } else {
    Serial.printf("[DEBUG SENSOR] Lettura Temp: %.2f C\n", t);
  }
}

void IRAM_ATTR read_bol(){
  sendBolle = true;
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
  msg = "termostato/";
  msg += termostato;
  mesh.sendSingle(to, msg);
}

void start_lcd(){
  lcd.setContrast(38); 
  lcd.print("Daisy24");
  delay(1000);
  lcd.setDoubleFont();
  delay(1000);
  lcd.setSingleFont();
  lcd.clear();
  lcd.print("Ctrl Frigorifero"); 
  lcd.lightON(pcf_add);  
}

void data_lcd(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp C");
  lcd.setCursor(12,0);
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("Temp Imp C");
  lcd.setCursor(12,1);
  lcd.print(termostato);
}

void turn_off_lcd() {
  // Se è acceso e sono passati 10 minuti (600000 ms)
  if(isLcdOn && (millis() - m) > 600000) {
    lcd.lightOFF(pcf_add); 
    isLcdOn = false;
  }
}

void update_temperature() {
  read_dallas();
  float isteresi = 0.5; 
  bool relayCambiato = false; // Flag di controllo
  
  if(t >= (termostato + isteresi) && relayState == LOW){
    digitalWrite(relay, HIGH);   
    relayState = HIGH;
    relayCambiato = true;
  }
  else if(t <= (termostato - isteresi) && relayState == HIGH){
    digitalWrite(relay, LOW);   
    relayState = LOW;
    relayCambiato = true;
  }

  // AGGIORNAMENTO VISIVO: Se lo schermo è acceso, aggiorna i gradi in tempo reale
  if (isLcdOn) {
    data_lcd();
  }

  // RIGORE DI RETE: Invia il pacchetto MQTT del relè SOLO se è appena scattato
  if (relayCambiato) {
    msg = "output/";
    msg += (relayState == HIGH) ? "1" : "0";
    mesh.sendSingle(to, msg);
    Serial.printf("[TERMOSTATO] Relè commutato: %d. Temp attuale: %.2f\n", relayState, t);
  }
}

void readButtons() {
  if (Update.isRunning()) return; 

  // --- CONTROLLO DI STATO I2C (GATING) ---
  // Inseriamo un controllo per evitare il crash del bus
  // Wire.endTransmission() ritorna 0 se il dispositivo risponde
  Wire.beginTransmission(pcf_add);
  if (Wire.endTransmission() != 0) {
     // Bus non pronto o dispositivo non rilevato, esci subito
     return; 
  }
  // --- FINE CONTROLLO ---
  byte tasti = lcd.ReadKey(pcf_add); 
  
  if (tasti == statoPrecedenteTasti) return;
  if (millis() - debounceTimer < 50) return; 
  
  debounceTimer = millis();
  statoPrecedenteTasti = tasti;

  // Ignora stato di riposo (15) o rumore elettrico estremo
  if (tasti == 15 || tasti == 0 || tasti == 255) return; 

  // --- REGOLA DI RESILIENZA: RISVEGLIO SICURO ---
  // Se lo schermo è spento, la prima pressione lo accende e disinnesca l'azione.
  if (!isLcdOn) {
    lcd.lightON(pcf_add);
    isLcdOn = true;
    m = millis();
    data_lcd(); // Mostra subito le temperature
    return; // Abortisce la funzione. Il tasto non agisce sui gradi.
  }

  // Se siamo qui, lo schermo era già acceso. Processiamo i comandi.
  m = millis(); // Riavvia il timer di auto-spegnimento

  switch (tasti) {
    case 0b00001101: // TOP-RIGHT: AUMENTA
      termostato++;
      if (termostato > 40) termostato = 40; // Limite meccanico superiore
      pendingSave = true;
      lastSetPointTime = millis();
      data_lcd(); 
      break;

    case 0b00001011: // BOTTOM-RIGHT: DIMINUISCE
      termostato--;
      if (termostato < 1) termostato = 1; // Limite meccanico inferiore
      pendingSave = true;
      lastSetPointTime = millis();
      data_lcd(); 
      break;

    case 0b00001110: // TOP-LEFT: SPEGNIMENTO MANUALE
      lcd.lightOFF(pcf_add);
      isLcdOn = false;
      return; // Esce senza aggiornare le scritte a display spento

    case 0b00000111: // BOTTOM-LEFT: TELEMETRIA MESH (INFO)
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ID:");
      lcd.setCursor(4,0);
      lcd.print(mesh.getNodeId());
      lcd.setCursor(0,1);
      lcd.print("Mem:");
      lcd.setCursor(5,1);
      lcd.print(ESP.getFreeHeap());
      // Non richiamo data_lcd() qui. Tornerà al menu principale
      // al prossimo giro del taskSendMessage (dopo ~100 secondi) 
      // o alla prossima pressione di risveglio.
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("[STARTUP] Inizializzazione Hardware ESP32-C3...");
  
  sensors.begin();
  Wire.begin(I2C_SDA, I2C_SCL); 
  lcd.begin();  
  
  pinMode(relay, OUTPUT);
  pinMode(bolle, INPUT_PULLUP); 
  digitalWrite(relay, LOW); 
  
  EEPROM.begin(512);
  EEPROM.get(0, termostato); // get() legge i 4 byte corretti
  
  // Paracadute Logico: se legge spazzatura, forza 20 gradi
  if(termostato < 1 || termostato > 40) {
    Serial.printf("[STARTUP] Memoria instabile (Letto: %d). Forzo 20 C.\n", termostato);
    termostato = 20;
  } else {
    Serial.printf("[STARTUP] Termostato recuperato da EEPROM: %d C\n", termostato);
  }
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 11 );
  mesh.initOTAReceive(ROLE);
  mesh.setContainsRoot(true);
  mesh.setName(nodeName);
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( retryRootTask ); // Registrazione nel core dello Scheduler
  taskSendMessage.enable();
  userScheduler.addTask( taskReadButtons );
  taskReadButtons.enable();
  // INIEZIONE: Attivazione del core termico
  userScheduler.addTask( taskControlloTermico );
  taskControlloTermico.enable();
  
  attachInterrupt(digitalPinToInterrupt(bolle), read_bol, FALLING);

  start_lcd();
  Serial.println("[STARTUP] Sistema Online e Operativo.");
  
}

void loop() {
  mesh.update();
  
  // Rilevamento Bolle
  if (sendBolle == true){
    String msgBolla = "bolle/";
    msgBolla += "1";
    mesh.sendSingle(to, msgBolla);
    Serial.println("[DEBUG EVENT] Rilevata bolla su IO1. Inviato.");
    sendBolle = false;
  }

  // Motore Asincrono di Salvataggio EEPROM
  if (pendingSave && (millis() - lastSetPointTime > SAVE_DELAY)) {
    EEPROM.put(0, termostato); 
    EEPROM.commit();           
    pendingSave = false;       
    Serial.printf("[DEBUG EEPROM] Consolidamento completato. Target %d C blindato in Flash.\n", termostato);

    // INIEZIONE OPENHAB: Spara il dato aggiornato verso il Bridge MQTT
    String syncMsg = "termostato/";
    syncMsg += termostato;
    mesh.sendSingle(to, syncMsg);
  }
}