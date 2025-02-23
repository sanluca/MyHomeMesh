//************************************************************
// read temperature  "tenda" ESP32C3 Dev Module
//
// pin vento D5 --> 14
//SHT30 i2c D1 D2
//pin salita D0 --> 16
//pin discesa D7 --> 13
//painlessmesh 1.5.3 arduinojson 6.21.5 esp32 3.0.7
//
//************************************************************
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
//void ICACHE_RAM_ATTR read_wind(); //per esp8266
void IRAM_ATTR read_wind();//per esp32

float t,h;
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry
int Statoreed = 0; // variabile per la lettura del contatto
int Statoreed_old = 0; // variabile per evitare doppio conteggio
int impulsi = 0;
unsigned long int TempoStart = 0; // memorizza i  millisecondi dalla prima pulsazione conteggiata   
unsigned long int Tempo = 0; // per conteggiare il tempo trascorso dalla prima pulsazione   
unsigned long int TempoMax = 2000;// numero di millisecondi (2 secondi) per azzerare il conteggio e calcolare la Velocità
const float Pi = 3.141593; // Pigreco
const byte relay_salita = 0;
const byte relay_discesa = 1;
const int vento = 2;
const float raggio = 0.06; // raggio dell'anemometro in metri
float deltaTempo = 0;
float Metris = 0;
float Kmora= 0;
float imp = 8; //velocità max poi si apre la tenda
volatile boolean leggiVento=false;


//#include "painlessMesh.h"
#include "../namedmesh/namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "tenda"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "tenda"
#define VERSION "Tenda v3.1.0"
#define MESSAGE "tenda "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1();
void retryRoot();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task retryRootTask(RETRY_ROOT_INTERVAL, TASK_ONCE, &retryRoot);

void sendMessage() {
  read_sht30();
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
      digitalWrite(relay_salita, HIGH);
      delay(1000);
      digitalWrite(relay_salita, LOW);
      msg = "output/1";
      mesh.sendSingle(to, msg);
   }
   else if (strcmp(rel.c_str(),"2") == 0) {
    digitalWrite(relay_discesa, HIGH);
    delay(1000);
    digitalWrite(relay_discesa, LOW);
    msg = "output/2";
    mesh.sendSingle(to, msg);
  } 
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> tenda: New Connection, nodeId = %u\n", nodeId);
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

void read_sht30(){
  t=sht31.readTemperature();
  h=sht31.readHumidity();
  if (isnan(t)) {
          //Serial.println("Failed to read from DHT sensor!");
          msg = "error/FailedtoreadfromSHTsensor";
          mesh.sendSingle(to, msg);
          return;
}}

void read_wind() {
  leggiVento=true;
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
  pinMode(relay_salita, OUTPUT);
  digitalWrite(relay_salita, LOW);
  pinMode(relay_discesa, OUTPUT);
  digitalWrite(relay_discesa, LOW);
  sht31.begin(0x44);
  
  pinMode(vento, INPUT);
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
  
  attachInterrupt(digitalPinToInterrupt(vento), read_wind, FALLING);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
 
 if (leggiVento==true){
  //Serial.printf("impulsi = %u\n", impulsi);
  leggiVento=false;

  if (impulsi == 0)// se E' il primo passaggio si memorizza il tempo di partenza
    { 
      TempoStart =  millis();
    } 
  
  impulsi = impulsi + 1; // si aggiorna il contatore 
  Tempo = ( millis() - TempoStart); // si conteggia il tempo trascorso dallo start conteggio
  if (Tempo >=  TempoMax){   // se il tempo trascorso è maggiore o uguale al tempo impostato si eseguono i calcoli e la stampa della velocità
    deltaTempo = ( Tempo/1000.0); // si trasforma in secondi
    Metris= (impulsi*2*Pi*raggio)/deltaTempo;      // si calcola la velocità in metri/s
    Kmora = (3.6*impulsi*Pi*raggio)/deltaTempo; //formula per il calcolo della velocità in Km/h
    impulsi =0;
    msg = "vento/";
    msg += Kmora;
    mesh.sendSingle(to, msg);
  }
   
  if (int(Kmora)>int(imp)){
    digitalWrite(relay_salita, HIGH);
    //leggiVento=false;
    //Serial.printf("reed non attivo\n");
    delay(1000);
    digitalWrite(relay_salita, LOW); 
    msg = "output/1";
    mesh.sendSingle(to, msg);
  }
}}