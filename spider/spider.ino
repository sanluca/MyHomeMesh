//spider adeept ADA033-V5.0
//moficare servo.h in 13 servo per timer
#include "movimento_robot.h"
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_NeoPixel.h>    
#define led_numbers  6 //WS2812 number  of LED
#define PIN  A1  //WS2812 PIN                                        
Adafruit_NeoPixel strip = Adafruit_NeoPixel(led_numbers, PIN, NEO_GRB + NEO_KHZ800);//NEO_KHZ400+NEO_RGB

MPU6050 mpu(Wire);

long timer = 0;

const int pingPin = A3;  // pin connected to Echo Pin in the ultrasonic distance sensor
const int trigPin = A2;  // pin connected to trig Pin in the ultrasonic distance sensor
int cm = 0;

//Rosso: R=255, G=0, B=0
//Verde: R=0, G=255, B=0
//Blu: R=0, G=0, B=255
//Giallo: R=255, G=255, B=0 (rosso + verde)
//Magenta: R=255, G=0, B=255 (rosso + blu)
//Ciano: R=0, G=255, B=255 (verde + blu)
//Bianco: R=255, G=255, B=255 (tutti i colori alla massima intensità)
//Nero: R=0, G=0, B=0 (assenza di tutti i colori)
void led(int R, int G, int B) {
  for (int i = 0; i < led_numbers; i++) {
    strip.setPixelColor(i, strip.Color(R, G, B));
    strip.show();
    delay(50);
  }
}

int ping(int pingPin) { 
   // establish variables for duration of the ping, 
   // and the distance result in inches and centimeters: 
   long duration, cm; 
   // The PING))) is triggered by a HIGH pulse of 2 or more microseconds. 
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse: 
   pinMode(trigPin, OUTPUT); 
   digitalWrite(trigPin, LOW); 
   delayMicroseconds(2); 
   digitalWrite(trigPin, HIGH); 
   delayMicroseconds(5); 
   digitalWrite(trigPin, LOW); 

   pinMode(pingPin, INPUT); 
   duration = pulseIn(pingPin, HIGH); 

   // convert the time into a distance 
   cm = microsecondsToCentimeters(duration); 
   return cm ; 
} 

long microsecondsToCentimeters(long microseconds) { 
   // The speed of sound is 340 m/s or 29 microseconds per centimeter. 
   // The ping travels out and back, so to find the distance of the 
   // object we take half of the distance travelled. 
   return microseconds / 29 / 2; 
}

void distanza() {
   cm = ping(pingPin); 
   Serial.print("distance: "); // Print a message of "Temp: "to the serial montiol.
   Serial.print(cm);           // Print a centigrade temperature to the serial montiol. 
   Serial.println(" cm");      // Print the unit of the centigrade temperature to the serial montiol.
   delay(500);
}

void mpu6050() {

  Serial.print(F("TEMPERATURE: "));Serial.println(mpu.getTemp());
  Serial.print(F("ACCELERO  X: "));Serial.print(mpu.getAccX());
  Serial.print("\tY: ");Serial.print(mpu.getAccY());
  Serial.print("\tZ: ");Serial.println(mpu.getAccZ());
  
  Serial.print(F("GYRO      X: "));Serial.print(mpu.getGyroX());
  Serial.print("\tY: ");Serial.print(mpu.getGyroY());
  Serial.print("\tZ: ");Serial.println(mpu.getGyroZ());
  
  Serial.print(F("ACC ANGLE X: "));Serial.print(mpu.getAccAngleX());
  Serial.print("\tY: ");Serial.println(mpu.getAccAngleY());
    
  Serial.print(F("ANGLE     X: "));Serial.print(mpu.getAngleX());
  Serial.print("\tY: ");Serial.print(mpu.getAngleY());
  Serial.print("\tZ: ");Serial.println(mpu.getAngleZ());
  Serial.println(F("=====================================================\n"));

}

bool robotIncastrato() {
  mpu6050();

  float sogliaInclinazione = 0.01; // Soglia da calibrare

  float inclinazioneX = mpu.getAccY();//mpu.getAccAngleX();
  float inclinazioneY = mpu.getAccAngleY();
  //if (abs(inclinazioneX) > sogliaInclinazione || abs(inclinazioneY) > sogliaInclinazione) {
  if (inclinazioneX < sogliaInclinazione) {
    Serial.println("true");
    return true; // Robot incastrato
    
  } else {
    Serial.println("false");
    return false;
    
  }
}

void setup() {
  Serial.begin(9600);     //opens serial port, sets data rate to 9600 bps
  Wire.begin();
  mpu.begin();
  mpu.calcOffsets(true,true); // gyro and accelero
  attachServos(); 
  riposo();

  //Initialize WS2812
  strip.begin();
  //Set the WS2812 brightness
  strip.setBrightness(50);

  pinMode(pingPin, INPUT); //Set the connection pin output mode Echo pin
  pinMode(trigPin, OUTPUT);//Set the connection pin output mode trog pin

}



void loop() {
  distanza(); // Misura la distanza dall'ostacolo
  led(0, 255, 0);
  mpu.update();

  //if (robotIncastrato()) { // <--- Controlla se il robot è incastrato *prima* di tutto
    Serial.println("Robot incastrato!");
    indietro(); // Esempio: vai indietro di un po'
    delay(1000);
    testa_dritta();
    delay(1000);
    gira_a_destra(); // Esempio: prova a girare di nuovo
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    distanza();
    delay(1000);

 // } else if (cm > 40) { // <--- Se non è incastrato, controlla la distanza
 if (cm > 40) {
    avanti(); // Se non ci sono ostacoli vicini, vai avanti
  } else {
    testa_destra(); // Altrimenti, muovi la testa a destra
    delay(1000); // Attendi un po' per dare tempo al servo di muoversi
    distanza(); // Misura di nuovo la distanza *dopo* aver mosso la testa

    if (cm > 40) {
      gira_a_destra(); // Se ora è libero a destra, gira a destra e vai avanti
      gira_a_destra();
      gira_a_destra();
      gira_a_destra();
      gira_a_destra();
      delay(1000);
      testa_dritta();
      delay(1000);
      avanti();
    } else {
      testa_sinistra(); // Se non è libero a destra, muovi la testa a sinistra
      delay(1000);
      distanza(); // Misura di nuovo la distanza *dopo* aver mosso la testa

      if (cm > 40) {
        gira_a_sinistra(); // Se ora è libero a sinistra, gira a sinistra e vai avanti
        gira_a_sinistra();
        gira_a_sinistra();
        gira_a_sinistra();
        gira_a_sinistra();
        delay(1000);
        testa_dritta();
        delay(1000);
        distanza();
        avanti();

      } else {
        // Se non è libero né a destra né a sinistra, puoi aggiungere un'azione alternativa
        // come andare indietro, fermarsi, o cercare un'altra direzione.
        indietro(); // Esempio: vai indietro di un po'
        delay(1000);
        testa_dritta();
        delay(1000);
        gira_a_destra(); // Esempio: prova a girare di nuovo
        gira_a_destra();
        gira_a_destra();
        gira_a_destra();
        gira_a_destra();
        distanza();
        delay(1000);
      }
    }
  }
}
