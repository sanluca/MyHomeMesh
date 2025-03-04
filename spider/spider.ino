//spider adeept ADA033-V5.0
//modificare servo.h in 13 servo per timer
#define NUM_CAMPIONI 5 // Numero di letture per media mobile
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
// se scrivo 7 li accende tutti
void led(int ledNumber, int R, int G, int B) {
    if (ledNumber >= 0 && ledNumber < led_numbers) { // Verifica che il numero del LED sia valido
        strip.setPixelColor(ledNumber, strip.Color(R, G, B));
        strip.show();
    } else if (ledNumber == 7) { // Se ledNumber è 7, accendi tutti i LED
        for (int i = 0; i < led_numbers; i++) {
            strip.setPixelColor(i, strip.Color(R, G, B));
        }
        strip.show();
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
   Serial.print("distance: "); 
   Serial.print(cm);           
   Serial.println(" cm");
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
  static float accX_media = 0; 
  static float accY_media = 0;
  static int count = 0;

  mpu6050(); // Aggiorna i dati del sensore

  float sogliaMovimento = 0.005; // Ridotto da 0.05 a 0.005
  float sogliaInclinazione = 10.0; // Aumentata per evitare falsi positivi
  float sogliaGiro = 0.5; // Se il giroscopio rileva un movimento significativo, il robot non è incastrato

  float accX = abs(mpu.getAccX());
  float accY = abs(mpu.getAccY());
  float gyroZ = abs(mpu.getGyroZ());
  float inclinazioneX = abs(mpu.getAccAngleX());
  float inclinazioneY = abs(mpu.getAccAngleY());

  // Aggiornamento della media mobile
  accX_media = ((accX_media * count) + accX) / (count + 1);
  accY_media = ((accY_media * count) + accY) / (count + 1);
  count = min(count + 1, NUM_CAMPIONI); // Evita overflow

  Serial.print("AccX Media: "); Serial.print(accX_media);
  Serial.print(" - AccY Media: "); Serial.print(accY_media);
  Serial.print(" - GyroZ: "); Serial.println(gyroZ);
  
  // Verifica se il robot è bloccato (bassa accelerazione per un periodo)
  bool bloccato = (accX_media < sogliaMovimento && accY_media < sogliaMovimento);

  // Se il giroscopio indica che sta ruotando, allora il robot NON è bloccato
  if (gyroZ > sogliaGiro) {
    Serial.println("Robot in movimento (sta girando).");
    return false;
  }

  // Verifica se è inclinato in modo anomalo
  bool inclinato = (inclinazioneX > sogliaInclinazione || inclinazioneY > sogliaInclinazione);

  if (bloccato || inclinato) {
    Serial.println("Robot INCASTRATO!");
    return true;
  }

  Serial.println("Robot in movimento.");
  return false;
}

void setup() {
  Serial.begin(9600);     //opens serial port, sets data rate to 9600 bps

  attachServos(); 
  riposo();
  delay(1000);
  Wire.begin();
  mpu.begin();
  mpu.calcOffsets(true,true); // gyro and accelero
  
  //Initialize WS2812
  strip.begin();
  //Set the WS2812 brightness
  strip.setBrightness(50);

  pinMode(pingPin, INPUT); //Set the connection pin output mode Echo pin
  pinMode(trigPin, OUTPUT);//Set the connection pin output mode trog pin

  led(1, 255, 0, 0);
  delay(1000);
  led(2, 0, 255, 0);
  delay(1000);
  led(3, 0, 0, 255);
  delay(1000);
  led(4, 255, 255, 0);
  delay(1000);
  led(5, 0, 255, 255);
  delay(1000);
  led(6, 255, 0, 255);

}



void loop() {
  distanza(); // Misura la distanza dall'ostacolo
  led(7, 0, 255, 0);
  mpu.update();

  if (robotIncastrato()) { // <--- Controlla se il robot è incastrato *prima* di tutto
    Serial.println("Robot incastrato!");
    led(7, 255, 0, 0);
    indietro(); // Esempio: vai indietro di un po'
    delay(1000);
    indietro();
    delay(1000);
    indietro();
    delay(1000);
    testa_dritta();
    delay(1000);
    gira_a_destra(); // Esempio: prova a girare di nuovo
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    distanza();
    delay(1000);

  } else if (cm > 40) { // <--- Se non è incastrato, controlla la distanza
    avanti(); // Se non ci sono ostacoli vicini, vai avanti
    distanza();
  } else {
    led(7, 0, 0, 255);
    testa_destra(); // Altrimenti, muovi la testa a destra
    delay(1000); // Attendi un po' per dare tempo al servo di muoversi
    distanza(); // Misura di nuovo la distanza *dopo* aver mosso la testa

    if (cm > 40) {
      led(7, 0, 0, 255);
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
      led(7, 0, 0, 255);
      testa_sinistra(); // Se non è libero a destra, muovi la testa a sinistra
      delay(1000);
      distanza(); // Misura di nuovo la distanza *dopo* aver mosso la testa

      if (cm > 40) {
        led(7, 0, 0, 255);
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
        delay(1000);

      } else {
        led(7, 0, 0, 255);
        // Se non è libero né a destra né a sinistra, puoi aggiungere un'azione alternativa
        // come andare indietro, fermarsi, o cercare un'altra direzione.
        indietro(); // Esempio: vai indietro di un po'
        delay(1000);
        indietro(); // Esempio: vai indietro di un po'
        delay(1000);
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
