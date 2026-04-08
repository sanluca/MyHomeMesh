//************************************************************
// read temperature  "tenda" ESP32C3 Dev Module
//
// pin vento 1
//SHT30 i2c 8 9
//pin salita 4
//pin discesa 3
//painlessmesh 1.5.7 arduinojson 7.4.2 esp32 2.0.17
//
//************************************************************
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <Preferences.h>

Preferences preferences;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
void IRAM_ATTR read_wind();//per esp32
volatile unsigned long lastWindInterruptTime = 0;
unsigned long windDebounceDelay = 30; // 30ms debounce
unsigned long lastAlarmTime = 0;      // Memoria ultimo scatto allarme
const unsigned long TempoInterdizione = 60000; // 60 secondi di attesa prima di poter ridare un comando vento
float t,h;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
int Statoreed = 0; // variabile per la lettura del contatto
int Statoreed_old = 0; // variabile per evitare doppio conteggio
volatile int impulsi = 0;
unsigned long int TempoStart = 0; // memorizza i  millisecondi dalla prima pulsazione conteggiata   
unsigned long int Tempo = 0; // per conteggiare il tempo trascorso dalla prima pulsazione   
unsigned long int TempoMax = 2000;// numero di millisecondi (2 secondi) per azzerare il conteggio e calcolare la Velocità
const float Pi = 3.141593; // Pigreco
const byte relay_salita = 4; //0
const byte relay_discesa = 3; //1
const int vento = 1;
const float raggio = 0.06; // raggio dell'anemometro in metri
float deltaTempo = 0;
float Metris = 0;
float Kmora= 0;
float imp = 7; //velocità max poi si apre la tenda
float old_Kmora = -1.0; // Parte da -1 così il primo "0" viene inviato

#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
namedMesh  mesh;
String nodeName = "tenda"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "tenda"
#define VERSION "Tenda v4.0.1"
#define MESSAGE "tenda "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();
void stopRelays();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);
// Task "OneShot" (monostabile) per spegnere i relè dopo 1000ms
Task taskStopRelay(1000, TASK_ONCE, &stopRelays);

void sendMessage() {
  String msg;
  read_sht30();

  char temp_str[10];
  char hum_str[10];
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
  String rel;
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if (root_id != 0 && from != 0 && root_id != from) {
      delay(200); 
      ESP.restart();
  }
  
  rel=msg;
  root_id=from;

 if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay_salita, HIGH);
      taskStopRelay.restartDelayed();
      msg = "output/1";
      mesh.sendSingle(to, msg);
  }
  else if (strcmp(rel.c_str(),"2") == 0) {
    digitalWrite(relay_discesa, HIGH);
    taskStopRelay.restartDelayed();
    msg = "output/2";
    mesh.sendSingle(to, msg);
  } 

  // NUOVO COMANDO: set_limit/12.5
  else if (msg.startsWith("set_limit/")) {
      String valStr = msg.substring(10); // Prende il numero dopo "set_limit/"
      float nuovoValore = valStr.toFloat();

      // 1. Sanity Check: Accettiamo solo valori sensati (es. tra 1 e 50)
      if (nuovoValore >= 1.0 && nuovoValore <= 50.0) {
          
          // 2. PROTEZIONE FLASH: Scriviamo SOLO se il valore è diverso dall'attuale
          // Usiamo una piccola tolleranza (0.01) perché i float non sono mai perfetti
          if (abs(imp - nuovoValore) > 0.01) {
              imp = nuovoValore;
              preferences.putFloat("limite_vento", imp); // SCRITTURA FISICA (Solo qui!)
              
              String risposta = "info/NuovoLimiteSalvato";
              mesh.sendSingle(to, risposta);
              Serial.println("Valore cambiato e salvato in Flash.");
          } else {
              // Se il valore è uguale, NON scriviamo in flash ma rispondiamo OK
              String risposta = "info/LimiteGiaImpostato";
              mesh.sendSingle(to, risposta);
              Serial.println("Valore identico. Scrittura Flash saltata.");
          }
      } else {
          String risposta = "error/ValoreFuoriRange";
          mesh.sendSingle(to, risposta);
      }
  }

}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> tenda: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
 Serial.println("changedConnectionCallback");
  if (!mesh.isConnected(root_id)) {
    //Serial.println("Connessione al root persa! Pianifico il retry.");
    if (!retryTaskEnabled) { // Controlla il flag
      retryRootTask.enable();
      retryTaskEnabled = true; // Imposta il flag
    }
  } else {
    //Serial.println("Connessione al root ripristinata.");
    retryRootTask.disable();
    retryTaskEnabled = false; // Resetta il flag
  }}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void read_sht30(){
  String msg;
  t=sht31.readTemperature();
  h=sht31.readHumidity();
  if (isnan(t)) {
          //Serial.println("Failed to read from DHT sensor!");
          msg = "error/FailedtoreadfromSHTsensor";
          mesh.sendSingle(to, msg);
          return;
}}

void read_wind() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastWindInterruptTime > windDebounceDelay) {
    impulsi++;
    lastWindInterruptTime = interruptTime;
  }
}

void update_status() {
  String msg;
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
  // Invia il limite attuale così sai sempre a quanto è tarata
  msg = "limite_vento/";
  msg += String(imp);
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

void stopRelays() {
  digitalWrite(relay_salita, LOW);
  digitalWrite(relay_discesa, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(relay_salita, OUTPUT);
  digitalWrite(relay_salita, LOW);
  pinMode(relay_discesa, OUTPUT);
  digitalWrite(relay_discesa, LOW);
  Wire.begin(8, 9);
  sht31.begin(0x44);
  pinMode(vento, INPUT_PULLUP);
  // --- GESTIONE MEMORIA ---
  preferences.begin("tenda_data", false); // Apre lo spazio di memoria "tenda_data"
  // Legge il valore salvato. Se non esiste (scheda nuova), usa 7.0
  imp = preferences.getFloat("limite_vento", 7.0); 
  Serial.print("Limite vento caricato da Flash: ");
  Serial.println(imp);
  // -------------------------
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
  userScheduler.addTask( taskStopRelay );
  taskSendMessage.enable();
  taskSendMessage1.enable();
  
  attachInterrupt(digitalPinToInterrupt(vento), read_wind, FALLING);
  TempoStart = millis();
  lastAlarmTime = millis() - TempoInterdizione;
}

void loop() {
  mesh.update();
  // CONTROLLO TEMPORALE (Indipendente dagli impulsi)
  if (millis() - TempoStart >= TempoMax) {
    
    // 1. Leggi e resetta il contatore proteggendolo dalle interruzioni
    noInterrupts();
    unsigned long letturaImpulsi = impulsi;
    impulsi = 0;
    interrupts();
    
    // 2. Calcola il tempo esatto trascorso
    deltaTempo = (millis() - TempoStart) / 1000.0;
    TempoStart = millis(); // Riavvia il cronometro

    // 3. Calcola Velocità
    if (letturaImpulsi > 0) {
      // Uso le tue formule originali per mantenere la tua taratura
      Metris = (letturaImpulsi * 2 * Pi * raggio) / deltaTempo;
      Kmora = (3.6 * letturaImpulsi * Pi * raggio) / deltaTempo;
    } else {
      Kmora = 0.0; // Se non ci sono impulsi, la velocità VA A ZERO!
    }

    // 4. Invia il dato alla rete
    if (abs(Kmora - old_Kmora) > 0.1) {
       String msg = "vento/";
       msg += Kmora;
       mesh.sendSingle(to, msg);
       
       old_Kmora = Kmora; // Memorizza il valore attuale
    }

    // 5. Logica Allarme
    if (Kmora > imp){
       // Controllo Interdizione: Sono passati 60 secondi dall'ultimo allarme?
      if (millis() - lastAlarmTime > TempoInterdizione) {
           
           // SI: Possiamo dare l'impulso di salita
           digitalWrite(relay_salita, HIGH);
           taskStopRelay.restartDelayed(); // Impulso di 1 secondo (Click)
           
           lastAlarmTime = millis(); // Blocca qualsiasi altro comando vento per 60 secondi
           
           String msgA = "output/1";
           mesh.sendSingle(to, msgA);
       } 
       // NO: Siamo nei 60 secondi di interdizione. 
       // NON facciamo nulla per evitare che un secondo impulso fermi la tenda.
    }
  }
}