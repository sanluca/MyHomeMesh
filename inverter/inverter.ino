//************************************************************
//
// read data from inverter mppsolat 4024MT "inverter"
// with pzem004t
// wemos d1 mini pro 4mb
//

//************************************************************
// QPI + CRC + cr
// String QPIGS = "\x51\x50\x49\x47\x53\xB7\xA9\x0D";
// String QPIWS = "\x51\x50\x49\x57\x53\xB4\xDA\x0D";
// String QDI = "\x51\x44\x49\x71\x1B\x0D";
// String QMOD = "\x51\x4D\x4F\x44\x49\xC1\x0D";
// String QVFW =  "\x51\x56\x46\x57\x62\x99\x0D";
// String QVFW2 = "\x51\x56\x46\x57\x32\xC3\xF5\x0D";
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
//esp8266
SoftwareSerial serialInv(D5, D6); // RX, TX 
PZEM004Tv30 pzem (D3, D4); //RX, TX
PZEM004Tv30 pzem1 (D7, D0);
unsigned long now;
char *val;

float voltin,curin,powerin,enerin,freqin,pfin,voltout,curout,powerout,enerout,freqout,pfout, pvpower, I, V;

//Inverter QPIGS: (BBB.B CC.C DDD.D EE.E FFFF GGGG HHH III JJ.JJ KKK OOO TTTT EEEE UUU.U WW.WW PPPPP b7b6b5b4b3b2b1b0 QQ VV MMMMM b10b9b8<CRC><cr>
//Inverter QPIWS: (BBB.B CC.C DDD.D EE.E FF.F HHHH IIII JJ.J KK.K JJ.J KK.K LL.L O PP QQ0 O P Q R SS T U VV.V W X YYY Z<CRC><cr>

static uint8_t pipInputBuf[500];
int pipInputPointer = 0;
struct
    {
    char gridVoltage[16];
    char gridFrequency[16];
    char acOutput[16];
    char acFrequency[16];
    char acApparentPower[16];
    char acActivePower[16];
    char loadPercent[16];
    char busVoltage[16];
    char batteryVoltage[16];
    char batteryChargeCurrent[16];
    char batteryCharge[16];
    char inverterTemperature[16];
    char PVCurrent[16];
    char PVVoltage[16];
    char batterySCC[16];
    char batteryDischarge[16];
    char inverterStatus[16];
    char batteryVoltageFans[16];
    char eeprom[16];
    char PVChargingPower[16];
    char inverterStatus2[16];
    } pipVals;

//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg,rel;
String nodeName = "inverter"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id =0;

#define ROLE    "inverter"
#define VERSION "Inverter v1.0.10"
#define MESSAGE "inverter "

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sendMessage1() ;
void sendMessage2();
void sendMessage3();
void sendMessage4();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendMessage1( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage1 );
Task taskSendMessage2( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage2 );
Task taskSendMessage3( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage3 );
Task taskSendMessage4( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage4 );

void sendMessage() {
  read_inverter_QPIGS();
  msg = "gridvoltage/";
  msg += atof(pipVals.gridVoltage);
  mesh.sendSingle(to, msg);
  msg = "gridfrequency/";
  msg += atof(pipVals.gridFrequency);
  mesh.sendSingle(to, msg);
  msg = "acOutput/";
  msg += atof(pipVals.acOutput);
  mesh.sendSingle(to, msg);
  msg = "acFrequency/";
  msg += atof(pipVals.acFrequency);
  mesh.sendSingle(to, msg);
  msg = "acApparentPower/";
  msg += atof(pipVals.acApparentPower);
  mesh.sendSingle(to, msg);
  msg = "acActivePower/";
  msg += atof(pipVals.acActivePower);
  mesh.sendSingle(to, msg);
  msg = "loadPercent/";
  msg += atof(pipVals.loadPercent);
  mesh.sendSingle(to, msg);
  msg = "busVoltage/";
  msg += atof(pipVals.busVoltage);
  mesh.sendSingle(to, msg);
  msg = "batteryVoltage/";
  msg += atof(pipVals.batteryVoltage);
  mesh.sendSingle(to, msg);
  msg = "batteryChargeCurrent/";
  msg += atof(pipVals.batteryChargeCurrent);
  mesh.sendSingle(to, msg);
  msg = "batteryCharge/";
  msg += atof(pipVals.batteryCharge);
  mesh.sendSingle(to, msg);
  msg = "inverterTemperature/";
  msg += atof(pipVals.inverterTemperature);
  mesh.sendSingle(to, msg);
  msg = "PVCurrent/";
  msg += atof(pipVals.PVCurrent);
  mesh.sendSingle(to, msg);
  msg = "PVVoltage/";
  msg += atof(pipVals.PVVoltage);
  mesh.sendSingle(to, msg);
  msg = "batterySCC/";
  msg += atof(pipVals.batterySCC);
  mesh.sendSingle(to, msg);
  msg = "batteryDischarge/";
  msg += atof(pipVals.batteryDischarge);
  mesh.sendSingle(to, msg);
  msg = "inverterStatus/";
  msg += atof(pipVals.inverterStatus);
  mesh.sendSingle(to, msg);
  msg = "batteryVoltageFans/";
  msg += atof(pipVals.batteryVoltageFans);
  mesh.sendSingle(to, msg);
  msg = "eeprom/";
  msg += atof(pipVals.eeprom);
  mesh.sendSingle(to, msg);
  msg = "PVChargingPower/";
  msg += atof(pipVals.PVChargingPower);
  mesh.sendSingle(to, msg);
  msg = "inverterStatus2/";
  msg += atof(pipVals.inverterStatus2);
  mesh.sendSingle(to, msg);

  I = atof(pipVals.PVCurrent);
  V = atof(pipVals.PVVoltage);
  pvpower = I * V;
  msg = "PVPower/";
  msg += pvpower;
  mesh.sendSingle(to, msg);

  taskSendMessage.setInterval( random( TASK_SECOND * 100, TASK_SECOND * 200 ));
}

void sendMessage1() {
  //in inverter
  read_enel_in();
  //out inverter
  read_enel_out();
  taskSendMessage1.setInterval( random( TASK_SECOND * 12, TASK_SECOND * 16 ));
}

void sendMessage2() {
  update_status();
  taskSendMessage2.setInterval( random( TASK_SECOND * 450, TASK_SECOND * 550 ));
}

void sendMessage3() {
  read_inverter_QPIWS();

  taskSendMessage3.setInterval( random( TASK_SECOND * 600, TASK_SECOND * 700 ));
}

void sendMessage4() {
  read_inverter_QMOD();

  taskSendMessage4.setInterval( random( TASK_SECOND * 750, TASK_SECOND * 850 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel=msg;
  root_id=from;
  if (strcmp(rel.c_str(),"1") == 0){
    pzem.resetEnergy();
    pzem1.resetEnergy();
}}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

//read two pzem in out
void read_enel_in() {
  voltin = pzem.voltage();
  curin = pzem.current();
  powerin = pzem.power();
  enerin = pzem.energy();
  freqin = pzem.frequency();
  pfin = pzem.pf();
  if (!(isnan(voltin))){
  msg = "voltin/";
  msg += voltin;
  mesh.sendSingle(to, msg);
  msg = "curin/";
  msg += curin;
  mesh.sendSingle(to, msg);
  msg = "powerin/";
  msg += powerin;
  mesh.sendSingle(to, msg);
  msg = "enerin/";
  msg += enerin;
  mesh.sendSingle(to, msg);
  msg = "freqin/";
  msg += freqin;
  mesh.sendSingle(to, msg);
  msg = "pfin/";
  msg += pfin;
  mesh.sendSingle(to, msg);
}}

void read_enel_out() {
  voltout = pzem1.voltage();
  curout = pzem1.current();
  powerout = pzem1.power();
  enerout = pzem1.energy();
  freqout = pzem1.frequency();
  pfout = pzem1.pf();
  if (!(isnan(voltout))){
  msg = "voltout/";
  msg += voltout;
  mesh.sendSingle(to, msg);
  msg = "curout/";
  msg += curout;
  mesh.sendSingle(to, msg);
  msg = "powerout/";
  msg += powerout;
  mesh.sendSingle(to, msg);
  msg = "enerout/";
  msg += enerout;
  mesh.sendSingle(to, msg);
  msg = "freqout/";
  msg += freqout;
  mesh.sendSingle(to, msg);
  msg = "pfout/";
  msg += pfout;
  mesh.sendSingle(to, msg);
  }}

void read_inverter_QPIGS(){
  //(BBB.B CC.C DDD.D EE.E FFFF GGGG HHH III JJ.JJ KKK OOO TTTT EEEE UUU.U WW.WW PPPPP b7b6b5b4b3b2b1b0 QQ VV MMMMM b10b9b8<CRC><cr>
  String QPIGS = "\x51\x50\x49\x47\x53\xB7\xA9\x0D";
  String stringOne;
  serialInv.print(QPIGS);
  stringOne = serialInv.readStringUntil('\r');
  if (stringOne != NULL){

  // Now split the packet into the values
  char* charString = (char*) malloc((stringOne.length() + 1) * sizeof(char));
  strcpy(charString, stringOne.c_str());

  char* val = strtok(charString, " ");
  strcpy(pipVals.gridVoltage, val + 1); // Skip the initial '('

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.gridFrequency, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.acOutput, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.acFrequency, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.acApparentPower, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.acActivePower, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.loadPercent, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.busVoltage, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batteryVoltage, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batteryChargeCurrent, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batteryCharge, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.inverterTemperature, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.PVCurrent, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.PVVoltage, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batterySCC, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batteryDischarge, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.inverterStatus, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.batteryVoltageFans, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.eeprom, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.PVChargingPower, val);

  val = strtok(0, " "); // Get the next value
  strcpy(pipVals.inverterStatus2, val);

  free(charString);

  stringOne = "";
}}

void read_inverter_QPIWS(){
  //(BBB.B CC.C DDD.D EE.E FF.F HHHH IIII JJ.J KK.K JJ.J KK.K LL.L O PP QQ0 O P Q R SS T U VV.V W X YYY Z<CRC><cr>
  String QPIWS = "\x51\x50\x49\x57\x53\xB4\xDA\x0D";
  String stringOne;
  serialInv.print(QPIWS);
  stringOne = serialInv.readStringUntil('\r');
  if (stringOne != NULL){

  // Now split the packet into the values
  char* charString = (char*) malloc((stringOne.length() + 1) * sizeof(char));
  strcpy(charString, stringOne.c_str());

  char pipstatus[40];
  char* val = strtok(charString, " ");
  strcpy(pipstatus, val + 1); // Skip the initial '('

  //clean status
  msg = "status/";
  msg += "None";
  mesh.sendSingle(to, msg);

  // Now send the various PIP status messages - CONVERT TO JSON FOR USE
  if (pipstatus[1] == '1'){
    msg = "status/";
    msg += "Inverter-Fault";
    mesh.sendSingle(to, msg);}

  if (pipstatus[2] == '1'){
    msg = "status/";
    msg += "Bus-Over-Voltage";
    mesh.sendSingle(to, msg);}

  if (pipstatus[3] == '1'){
    msg = "status/";
    msg += "Bus-Under-Voltage";
    mesh.sendSingle(to, msg);}

  if (pipstatus[4] == '1'){
    msg = "status/";
    msg += "Bus-Soft-Fail";
    mesh.sendSingle(to, msg);}

  if (pipstatus[5] == '1'){
    msg = "status/";
    msg += "Line-Fail";
    mesh.sendSingle(to, msg);}

  if (pipstatus[6] == '1'){
    msg = "status/";
    msg += "OPV-Short";
    mesh.sendSingle(to, msg);}

  if (pipstatus[7] == '1'){
    msg = "status/";
    msg += "Inverter-Voltage-Too-Low";
    mesh.sendSingle(to, msg);}

  if (pipstatus[8] == '1'){
    msg = "status/";
    msg += "Inverter-Voltage-Too-High";
    mesh.sendSingle(to, msg);}

  if (pipstatus[9] == '1'){
    msg = "status/";
    msg += "Over-Temperature";
    mesh.sendSingle(to, msg);}

  if (pipstatus[10] == '1'){
    msg = "status/";
    msg += "Fan-Locked";
    mesh.sendSingle(to, msg);}

  if (pipstatus[11] == '1'){
    msg = "status/";
    msg += "Battery-Voltage-Too-High";
    mesh.sendSingle(to, msg);}

  if (pipstatus[12] == '1'){
    msg = "status/";
    msg += "Battery-Low-Alarm";
    mesh.sendSingle(to, msg);}

  if (pipstatus[14] == '1'){
    msg = "status/";
    msg += "Battery-Under-Shutdown";
    mesh.sendSingle(to, msg);}

  if ((pipstatus[16] == '1') && (pipstatus[1] == '0')){
    msg = "status/";
    msg += "Overload-Warning";
    mesh.sendSingle(to, msg);}

  if ((pipstatus[16] == '1') && (pipstatus[1] == '1')){
    msg = "status/";
    msg += "Overload-FAULT";
    mesh.sendSingle(to, msg);}

  if (pipstatus[17] == '1'){
    msg = "status/";
    msg += "EEPROM-Fault";
    mesh.sendSingle(to, msg);}

  free(charString);

  stringOne = "";
}}

void read_inverter_QMOD(){
  String QMOD = "\x51\x4D\x4F\x44\x49\xC1\x0D";
  String stringOne;
  serialInv.print(QMOD);
  stringOne = serialInv.readStringUntil('\r');
  
  if (stringOne != "") {
    // Now split the packet into the values
    char* charString = (char*) malloc((stringOne.length() + 1) * sizeof(char));
    strcpy(charString, stringOne.c_str());

    char* val = strtok(charString, " ");
    char pipstatus[2];
    strcpy(pipstatus, val);

  // Now send the various PIP status messages - CONVERT TO JSON FOR USE
  if (pipstatus[1] == 'P'){
    msg = "mode/";
    msg += "Power-On-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'S'){
    msg = "mode/";
    msg += "Standby-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'L'){
    msg = "mode/";
    msg += "Line-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'B'){
    msg = "mode/";
    msg += "Battery-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'Y'){
    msg = "mode/";
    msg += "Bypass-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'E'){
    msg = "mode/";
    msg += "ECO-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'F'){
    msg = "mode/";
    msg += "Fault-Mode";
    mesh.sendSingle(to, msg);}

  if (pipstatus[1] == 'H'){
    msg = "mode/";
    msg += "Power-Saving-Mode";
    mesh.sendSingle(to, msg);}

  free(charString);

  stringOne = "";
}}

void update_status(){

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
  //Serial.println("WiFi signal: " + String(WiFi.RSSI()) + " db");
  msg = "wifisignal/";
  msg += String(WiFi.RSSI());
  mesh.sendSingle(to, msg);
}

void setup() {
  // Modbus communication runs at 115200 baud
  Serial.begin(115200);
  serialInv.begin(2400);

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
  userScheduler.addTask( taskSendMessage2 );
  userScheduler.addTask( taskSendMessage3 );
  userScheduler.addTask( taskSendMessage4 );
  taskSendMessage.enable();
  taskSendMessage1.enable();
  taskSendMessage2.enable();
  taskSendMessage3.enable();
  taskSendMessage4.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
 if((millis() - now) > 600000) { //600000 - 10 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}}