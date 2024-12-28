//************************************************************
// read temperature and humidity "garage"
// wemos d1 mini pro
//
// 
// dht D4--> 2
// 
// 
//read epever Tracer2210AN modbus serial TX - RX
//************************************************************
#include <ModbusMaster.h>
#include <DHT.h>
#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE, 20);
unsigned long noww;
int count=0;
//epever
//uint8_t result;
uint16_t result;
float pvv, pvi, pvp, bv, bi, bp, lcurrent, lpower,lvoltage, bremaining, btemp, h, t,battOverallCurrent, temptracer,consumed_energy_today,consumed_energy_month;
float consumed_energy_year,generate_energy_today,generate_energy_month,generate_energy_year,Discharging_percentage,Charging_percentage;
//int rtc1, rtc2, rtc3;
char buf[256];
ModbusMaster node;
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg;
String nodeName = "garage"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "garage"
#define VERSION "Garage v2.1.15"
#define MESSAGE "garage "
// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  read_dht();
  read_epever();
  msg = "temperatura/";
  msg += t;
  mesh.sendSingle(to, msg);
  msg = "umidita/";
  msg += h;
  mesh.sendSingle(to, msg);
  msg = "pvv/";
  msg += pvv;
  mesh.sendSingle(to, msg);
  msg = "pvi/";
  msg += pvi;
  mesh.sendSingle(to, msg);
  msg = "pvp/";
  msg += pvp;
  mesh.sendSingle(to, msg);
  msg = "bv/";
  msg += bv;
  mesh.sendSingle(to, msg);
  msg = "bi/";
  msg += bi;
  mesh.sendSingle(to, msg);
  msg = "bp/";
  msg += bp;
  mesh.sendSingle(to, msg);
  msg = "lvoltage/";
  msg += lvoltage;
  mesh.sendSingle(to, msg);
  msg = "lcurrent/";
  msg += lcurrent;
  mesh.sendSingle(to, msg);
  msg = "lpower/";
  msg += lpower;
  mesh.sendSingle(to, msg);
  msg = "bremaining/";
  msg += bremaining;
  mesh.sendSingle(to, msg);
  msg = "btemp/";
  msg += btemp;
  mesh.sendSingle(to, msg);
  msg = "temptracer/";
  msg += temptracer;
  mesh.sendSingle(to, msg);
  msg = "bover/";
  msg += battOverallCurrent;
  mesh.sendSingle(to, msg);
  msg = "cons_KWH_day/";
  msg += consumed_energy_today;
  mesh.sendSingle(to, msg);
  msg = "gen_KWH_day/";
  msg += generate_energy_today;
  mesh.sendSingle(to, msg);
  msg = "cons_KWH_month/";
  msg += consumed_energy_month;
  mesh.sendSingle(to, msg);
  msg = "gen_KWH_month/";
  msg += generate_energy_month;
  mesh.sendSingle(to, msg);
  msg = "cons_KWH_year/";
  msg += consumed_energy_year;
  mesh.sendSingle(to, msg);
  msg = "gen_KWH_year/";
  msg += generate_energy_year;
  mesh.sendSingle(to, msg);
  msg = "discharging/";
  msg += Discharging_percentage;
  mesh.sendSingle(to, msg);
  msg = "charging/";
  msg += Charging_percentage;
  mesh.sendSingle(to, msg);
  msg = "rtc/";
  msg += buf;
  mesh.sendSingle(to, msg);

  update_status();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 200, TASK_SECOND * 300 ));

}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  root_id = from;
  // Controlla se il messaggio inizia con "timentp="
  //timentp=16-06-2023-12:14
  if (msg.startsWith("timentp=")) {
    // Estrae i componenti della data e dell'ora dal messaggio nel formato "15-06-2023-17:50" li scrive nel time della scheda
     int giorno, mese, anno, ora, minuti;
    sscanf(msg.c_str(), "timentp=%d-%d-%d-%d:%d", &giorno, &mese, &anno, &ora, &minuti);
    struct tm tm;
    tm.tm_year = anno - 1900;
    tm.tm_mon = mese -1;
    tm.tm_mday = giorno;
    tm.tm_hour = ora;
    tm.tm_min = minuti;
    tm.tm_sec = 0;

    struct timeval now_local;
    now_local.tv_sec = mktime(&tm);
    now_local.tv_usec = 0;

    settimeofday(&now_local, NULL);
     // Get the current time
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);

    // Convert the time to Modbus registers
    //uint16_t seconds_minutes = (timeinfo->tm_min << 8) | timeinfo->tm_sec;
    // Convert the time to Modbus registers
    uint16_t seconds_minutes = (tm.tm_min << 8) | tm.tm_sec;
    uint16_t hour_day = (tm.tm_hour << 8) | tm.tm_mday;
    uint16_t month_year = (tm.tm_year << 8) | (tm.tm_mon);


    // Write the time to the Modbus registers
    node.setTransmitBuffer(0, seconds_minutes);
    node.setTransmitBuffer(1, hour_day);
    node.setTransmitBuffer(2, month_year);

  // Write the registers to the EPEver MPPT solar charger
    result = node.writeMultipleRegisters(0x9013, 3);
    if (result == node.ku8MBSuccess) {
      // Se la scrittura dei registri Modbus ha avuto successo, invia il messaggio ricevuto al destinatario specificato da 'to'
      msg = "timentp/";
      msg += "ok";
      mesh.sendSingle(to, msg);
    } else {
      // Se la scrittura dei registri Modbus ha fallito, invia un messaggio di errore al destinatario specificato da 'to'
      msg = "timentp/";
      msg += result;
      mesh.sendSingle(to, msg);
      
    }
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void read_epever(){
  result = node.readInputRegisters(0x3100, 6);
  if (result == node.ku8MBSuccess)
  {
    //PV Voltage
    pvv=node.getResponseBuffer(0x00) / 100.0f;
    //PV Current
    pvi=node.getResponseBuffer(0x01) / 100.0f;
    //PV Power
    pvp=node.getResponseBuffer(0x02) / 100.0f;
    //Battery Voltage
    bv=node.getResponseBuffer(0x04) / 100.0f;
    //Battery Charge Current
    bi=node.getResponseBuffer(0x05) / 100.0f;
  }
  result = node.readInputRegisters(0x3106, 2);
  if (result == node.ku8MBSuccess) {
    //Battery Charge Power
    bp = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16)  / 100.0f;
  }
  result = node.readInputRegisters(0x310C, 4);
  if (result == node.ku8MBSuccess) {
    //Load Voltage
    lvoltage = node.getResponseBuffer(0x00) / 100.0f;
    //Load Current
    lcurrent = node.getResponseBuffer(0x01) / 100.0f;
    //Load Power
    lpower = (node.getResponseBuffer(0x02) | node.getResponseBuffer(0x03) << 16) / 100.0f;
  }
  result = node.readInputRegisters(0x311A, 2);
  if (result == node.ku8MBSuccess) {
    //Battery Remaining %
    bremaining = node.getResponseBuffer(0x00) / 1.0f;
  }
  result = node.readInputRegisters(0x3110, 2);
  if (result == node.ku8MBSuccess) {
    //Battery Temperature
    btemp = node.getResponseBuffer(0x00) / 100.0f;
    //Temperature TRACER
    temptracer = node.getResponseBuffer(0x01) / 100.0f;
  }
  result = node.readInputRegisters(0x331B, 2);
  if (result == node.ku8MBSuccess) {
    //Battery Discharge Current
    battOverallCurrent = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16) / 100.0f;
  }

  result = node.readInputRegisters(0x3304, 6);
    if (result == node.ku8MBSuccess) {
      //consumo giornaliero
      consumed_energy_today = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16) / 100.0f;
      consumed_energy_month = (node.getResponseBuffer(0x02) | node.getResponseBuffer(0x03) << 16) / 100.0f;
      consumed_energy_year = (node.getResponseBuffer(0x04) | node.getResponseBuffer(0x05) << 16) / 100.0f;
    }
  result = node.readInputRegisters(0x330C, 6);
    if (result == node.ku8MBSuccess) {
      //produzione giornaliera
      generate_energy_today = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16) / 100.0f;
      generate_energy_month = (node.getResponseBuffer(0x02) | node.getResponseBuffer(0x03) << 16) / 100.0f;
      generate_energy_year = (node.getResponseBuffer(0x04) | node.getResponseBuffer(0x05) << 16) / 100.0f;
    }

    result = node.readHoldingRegisters(0x906D, 2);
    if (result == node.ku8MBSuccess) {
      Discharging_percentage= node.getResponseBuffer(0x00) / 100.0f;
    }
    
    result = node.readHoldingRegisters(0x906E, 2);
    if (result == node.ku8MBSuccess) {
      Charging_percentage= node.getResponseBuffer(0x00) / 100.0f;
    }

    result = node.readHoldingRegisters(0x9013, 3);
     // clock
  union {
    struct {
      uint8_t  s;
      uint8_t  m;
      uint8_t  h;
      uint8_t  d;
      uint8_t  M;
      uint8_t  y;
    } r;
    uint16_t buf[3];
  } rtc ;
    memset(rtc.buf, 0, sizeof(rtc.buf));
    if (result == node.ku8MBSuccess) {
      rtc.buf[0]= node.getResponseBuffer(0x00);
      rtc.buf[1]= node.getResponseBuffer(0x01);
      rtc.buf[2]= node.getResponseBuffer(0x02);
      sprintf(buf, "%02d:%02d:%02d %02d-%02d-20%02d" ,rtc.r.h , rtc.r.m , rtc.r.s, rtc.r.d, rtc.r.M, rtc.r.y);//rtc.r.y , rtc.r.M , rtc.r.d , rtc.r.h , rtc.r.m , rtc.r.s);
      }
}

void read_dht() {

  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from DHT sensor!");
          msg = "error/";
          msg += "FailedDHTsensor!";
          mesh.sendSingle(to, msg);
          return;
        }}

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

void setup() {
  // Modbus communication runs at 115200 baud
  Serial.begin(115200);
  Serial.setTimeout(1000);
  //Modbus slave ID 1
  node.begin(1, Serial);
  dht.begin();

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
  taskSendMessage.enable();
  //pinMode(A0, INPUT);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
 if((millis() - noww) > 600000) { //600000 - 10 min
    noww=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}
}
