// spider adeept ADA033-V5.0
// modificare servo.h in 13 servo per timer
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

// Variabile di stato per l'inseguimento
bool insegui = false;

// Funzione per impostare il colore dei LED
void led(int ledNumber, int R, int G, int B) {
  if (ledNumber >= 0 && ledNumber < led_numbers) {
    strip.setPixelColor(ledNumber, strip.Color(R, G, B));
    strip.show();
  } else if (ledNumber == 7) {
    for (int i = 0; i < led_numbers; i++) {
      strip.setPixelColor(i, strip.Color(R, G, B));
    }
    strip.show();
  }
}

// Funzione per misurare la distanza con il sensore a ultrasuoni
int ping(int pingPin) { 
  long duration, cm; 
  pinMode(trigPin, OUTPUT); 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(5); 
  digitalWrite(trigPin, LOW); 

  pinMode(pingPin, INPUT); 
  duration = pulseIn(pingPin, HIGH); 

  cm = microsecondsToCentimeters(duration); 
  return cm ; 
} 

// Funzione per convertire i microsecondi in centimetri
long microsecondsToCentimeters(long microseconds) { 
  return microseconds / 29 / 2; 
}

// Funzione per aggiornare e stampare i dati del sensore MPU6050
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

// Funzione per rilevare se il robot è incastrato
bool robotIncastrato() {
  static float accX_media = 0; 
  static float accY_media = 0;
  static int count = 0;

  mpu6050(); 

  float sogliaMovimento = 0.005; 
  float sogliaInclinazione = 10.0; 
  float sogliaGiro = 0.5; 

  float accX = abs(mpu.getAccX());
  float accY = abs(mpu.getAccY());
  float gyroZ = abs(mpu.getGyroZ());
  float inclinazioneX = abs(mpu.getAccAngleX());
  float inclinazioneY = abs(mpu.getAccAngleY());

  accX_media = ((accX_media * count) + accX) / (count + 1);
  accY_media = ((accY_media * count) + accY) / (count + 1);
  count = min(count + 1, NUM_CAMPIONI); 

  Serial.print("AccX Media: "); Serial.print(accX_media);
  Serial.print(" - AccY Media: "); Serial.print(accY_media);
  Serial.print(" - GyroZ: "); Serial.println(gyroZ);
  
  bool bloccato = (accX_media < sogliaMovimento && accY_media < sogliaMovimento);

  if (gyroZ > sogliaGiro) {
    Serial.println("Robot in movimento (sta girando).");
    return false;
  }

  bool inclinato = (inclinazioneX > sogliaInclinazione || inclinazioneY > sogliaInclinazione);

  if (bloccato || inclinato) {
    Serial.println("Robot INCASTRATO!");
    return true;
  }

  Serial.println("Robot in movimento.");
  return false;
}

void setup() {
  Serial.begin(9600); 

  attachServos(); 
  riposo();
  delay(1000);
  Wire.begin();
  mpu.begin();
  mpu.calcOffsets(true,true); 
  
  strip.begin();
  strip.setBrightness(50);

  pinMode(pingPin, INPUT); 
  pinMode(trigPin, OUTPUT);

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
  cm = ping(pingPin);
  Serial.print("distance: "); 
  Serial.print(cm);                   
  Serial.println(" cm");
  led(7, 0, 255, 0);
  mpu.update();

  if (robotIncastrato()) { 
    Serial.println("Robot incastrato!");
    led(7, 255, 0, 0);
    indietro(); 
    delay(1000);
    indietro();
    delay(1000);
    indietro();
    delay(1000);
    testa_dritta();
    delay(1000);
    gira_a_destra(); 
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    gira_a_destra();
    delay(1000);
  } else {
    if (cm < 40 && !insegui) {
      insegui = true;
      Serial.println("Inseguimento iniziato!");
    } else if (cm > 100 && insegui) {
      insegui = false;
      Serial.println("Inseguimento terminato!");
      riposo(); 
    }

    if (insegui) {
      if (cm > 40) {
        avanti();
        delay(500);
      } else {
        led(7, 0, 0, 255);
        testa_destra();
        delay(1000);

        if (cm > 40) {
          led(7, 0, 0, 255);
          gira_a_destra();
          gira_a_destra();
          gira_a_destra();
          gira_a_destra();
          gira_a_destra();
          delay(1000);
          testa_dritta();
          delay(1000);
          avanti();
          delay(500);
        } else {
          led(7, 0, 0, 255);
          testa_sinistra();
          delay(1000);

          if (cm > 40) {
            led(7, 0, 0, 255);
            gira_a_sinistra();
            gira_a_sinistra();
            gira_a_sinistra();
            gira_a_sinistra();
            gira_a_sinistra();
            delay(1000);
            testa_dritta();
            delay(1000);
            avanti();
            delay(500);
          } else {
            indietro(); 
            delay(1000);
            indietro(); 
            delay(1000);
            indietro(); 
            delay(1000);
            testa_dritta();
            delay(1000);
            gira_a_destra(); 
            gira_a_destra();
            gira_a_destra();
            gira_a_destra();
            gira_a_destra();
            delay(1000);
          }
        }
      }
    } else {
      riposo();
    }
  }
}