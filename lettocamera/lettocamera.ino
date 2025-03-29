//************************************************************
// lettura presenza nel letto con HX711
// wemos d1 mini 
// painlessmesh 1.5.3
// esp8266 3.1.2
// arduinojson 6.21.5
//************************************************************
#include <HX711.h>
// HX711 Pins
const int LOADCELL_DOUT_PIN = 2; // D4 Remember these are ESP GPIO pins, they are not the physical pins on the board.
const int LOADCELL_SCK_PIN = 3; //RX
int calibration_factor = 400; // Defines calibration factor we'll use for calibrating. se diminuisce aumenta il valore inversamente proporzionale
float reading; // Float for reading
float raw; // Float for raw value which can be useful
const unsigned long RETRY_ROOT_INTERVAL = 1200000; // 20 minuti
bool retryTaskEnabled = false; // Flag per il task di retry

HX711 scale;                          // Initiate HX711 library

//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "lettocamera"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id=0;

#define ROLE    "lettocamera"
#define VERSION "LettoCamera v3.0.3"
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
}

void sendMessage1() {
  update_status();
  taskSendMessage1.setInterval( random( TASK_SECOND * 600, TASK_SECOND * 700 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
  scale.power_up(); // Accendi l'HX711
  if (scale.wait_ready_timeout()) { // Attendi che la scala sia pronta con timeout
    scale.set_scale(calibration_factor); // Imposta il fattore di calibrazione
    reading = scale.get_units(10); // Leggi la scala in g/Kg
    raw = scale.read_average(5); // Leggi il valore raw dalla scala

    if (reading < 0) {
      reading = 0.00; // Imposta la lettura a 0 se è negativa
    }

    // Usa snprintf per formattare le stringhe in modo più efficiente
    char reading_str[20];
    char raw_str[20];
    snprintf(reading_str, sizeof(reading_str), "%.2f", reading); // Formatta con 2 decimali
    snprintf(raw_str, sizeof(raw_str), "%.0f", raw); // Formatta senza decimali

    msg = "peso/";
    msg += reading_str;
    mesh.sendSingle(to, msg);

    msg = "valuerawstr/";
    msg += raw_str;
    mesh.sendSingle(to, msg);
  } else {
    Serial.println("Errore: HX711 non pronto!");
    msg = "error/HX711notready";
    mesh.sendSingle(to, msg);
  }
  scale.power_down(); // Spegni l'HX711
}

void update_status()
{
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
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);   // Start scale on specified pins
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
}