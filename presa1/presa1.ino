//************************************************************
// presa_1 teckin SP22
//  esp8266-S3  
// configurazione arduino ide - generic esp8266 module
// 
// Sel --> 12
// CF1 --> 14
// relay --> 15
// button --> 13
// led blue --> 2
// led red --> 0
// CF --> 5
// free --> 4  dht??
// BL0937 chip sulla scheda
// https://docs.kaaiot.io/KAA/docs/v1.3.0/Tutorials/device-integration/hardware-guides/connect-blitzwolf-smart-socket/
//************************************************************
#include "HLW8012.h" //BL0937 modificata https://github.com/kaaproject/hlw8012
#include "Button2.h"
float t;
unsigned long now;

#define BUTTON_1        13

const byte relay_pin = 15;
const byte led_blue = 2;
const byte led_red = 0;
bool relayState = LOW;

Button2 btn1(BUTTON_1);
//#include "painlessMesh.h"
#include "namedMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
//painlessMesh  mesh;
namedMesh  mesh;
String msg;
String rel;
String rel2;
String nodeName = "presa1"; // Name needs to be unique
String to = "bridgemqtt";
uint32_t root_id;

#define ROLE    "presa1"
#define VERSION "Presa1 v2.0.0"
#define MESSAGE "presa1 "


//BL0937
#define SEL_PIN     12
#define CF1_PIN     14
#define CF_PIN      5
# define CURRENT_RESISTOR                0.007
# define VOLTAGE_RESISTOR_UPSTREAM       (5*200000)
# define VOLTAGE_RESISTOR_DOWNSTREAM     (4000)
#define CURRENT_MODE                    LOW

HLW8012 bl0937;

unsigned int active_pow = 0;
unsigned int voltage = 0;
double current = 0;
unsigned int apparent_power = 0;
double power_factor = 0;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {

  update_status();
  send_bl0937();
  
  taskSendMessage.setInterval( random( TASK_SECOND * 100, TASK_SECOND * 200 ));

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  rel=msg;
  root_id=from;
//  }
//void update_rele(){
  if (rel2 != rel){
    if (strcmp(rel.c_str(),"1") == 0){
      digitalWrite(relay_pin, HIGH);
      relayState = HIGH;
      digitalWrite(led_blue, HIGH);
      digitalWrite(led_red, LOW);
      msg = "output/";
      msg += "1";
      mesh.sendSingle(to, msg);
      Serial.println("1");
   }
   else if (strcmp(rel.c_str(),"0") == 0) {
    digitalWrite(relay_pin, LOW);
    relayState = LOW;
    digitalWrite(led_blue, LOW);
    digitalWrite(led_red, HIGH);
    msg = "output/";
    msg += "0";
    mesh.sendSingle(to, msg);
    Serial.println("0");
  } 
  rel2=rel;
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

void button_setup()
{
  btn1.setPressedHandler([](Button2 & b) 
    {
    //  Serial.println("button");
    //  Serial.println(relayState);
      relayState = !relayState;
      digitalWrite(relay_pin,relayState);
     if (relayState == HIGH){
      digitalWrite(led_blue, HIGH);
      digitalWrite(led_red, LOW);
      msg = "output/";
      msg += "1";
      mesh.sendSingle(to, msg);
     }
     else if (relayState == LOW){
      digitalWrite(led_blue, LOW);
      digitalWrite(led_red, HIGH);
      msg = "output/";
      msg += "0";
      mesh.sendSingle(to, msg);
     }
    });
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
  msg = "output/";
  msg += relayState;
  mesh.sendSingle(to, msg);
  msg = "ip/";
  msg += WiFi.localIP().toString();
  mesh.sendSingle(to, msg);
  
}

void send_bl0937() {
    active_pow = bl0937.getActivePower();
    voltage = bl0937.getVoltage();
    current = bl0937.getCurrent();
    apparent_power = bl0937.getApparentPower();
    
    bl0937.toggleMode();

    msg = "power/";
    msg += active_pow;
    mesh.sendSingle(to, msg);
    msg = "voltage/";
    msg += voltage;
    mesh.sendSingle(to, msg);
    msg = "current/";
    msg += current/100.00;
    mesh.sendSingle(to, msg);
    msg = "apparent_power/";
    msg += (float)apparent_power/1000;
    mesh.sendSingle(to, msg);

    
}

void setup() {
  Serial.begin(115200);
  pinMode(relay_pin, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_red, OUTPUT);
//bl0937
  bl0937.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, false, 500000);
  bl0937.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);  

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

  button_setup();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  btn1.loop();
  update_rele();
  if((millis() - now) > 600000) { //600000 - 10 min
    now=millis();
      if(!(mesh.isConnected(root_id))){
        Serial.printf("Reboot\n");
        ESP.restart();
    }}
}
