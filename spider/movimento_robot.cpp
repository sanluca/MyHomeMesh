#include "movimento_robot.h"
#include <Servo.h>
#include <Arduino.h>

int tempoPausa = 100;

Servo servo2;//1sxa
Servo servo3;//1sxb
Servo servo4;//2sxa
Servo servo5;//2sxb
Servo servo6;//3sxa
Servo servo7;//3sxb
Servo servo8;//1dxa
Servo servo9;//1dxb
Servo servo10;//2dxa
Servo servo11;//2dxb
Servo servo12;//3dxa
Servo servo13;//3dxb
Servo servo14;//testa

void attachServos() {
servo2.attach(2);
servo3.attach(3);
servo4.attach(4);
servo5.attach(5);
servo6.attach(6);
servo7.attach(7);
servo8.attach(8);
servo9.attach(9);
servo10.attach(10);
servo11.attach(11);
servo12.attach(12);
servo13.attach(13);
servo14.attach(A0);
}

void riposo() {
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
  servo6.write(90);
  servo7.write(90);
  servo8.write(90);
  servo9.write(90);
  servo10.write(90);
  servo11.write(90);
  servo12.write(90);
  servo13.write(90);
  servo14.write(90);
}

void avanti() {
  delay(tempoPausa);
  servo2.write(45);
  delay(tempoPausa);
  servo3.write(135);
  delay(tempoPausa);
  servo2.write(90);
  delay(tempoPausa);

  servo6.write(45);
  delay(tempoPausa);
  servo7.write(135);
  delay(tempoPausa);
  servo6.write(90);
  delay(tempoPausa);
  
  servo10.write(135);
  delay(tempoPausa);
  servo11.write(45);
  delay(tempoPausa);
  servo10.write(90);
  delay(tempoPausa);

  servo8.write(135);
  servo4.write(45);
  servo12.write(135);
  delay(tempoPausa);
  servo3.write(90);
  servo7.write(90);
  servo11.write(90);
  delay(tempoPausa);

  servo8.write(135);
  delay(tempoPausa);
  servo9.write(45);
  delay(tempoPausa);
  servo8.write(90);
  delay(tempoPausa);

  servo4.write(45);
  delay(tempoPausa);
  servo5.write(135);
  delay(tempoPausa);
  servo4.write(90);
  delay(tempoPausa);

  servo12.write(135);
  delay(tempoPausa);
  servo13.write(45);
  delay(tempoPausa);
  servo12.write(90);
  delay(tempoPausa);
  servo2.write(45);
  servo6.write(45);
  servo10.write(135);
  delay(tempoPausa);

  servo9.write(90);
  servo5.write(90);
  servo13.write(90);
  delay(tempoPausa);
}

void indietro() {
  delay(tempoPausa);
  // zampa 1sx indietro
  servo2.write(45);
  delay(tempoPausa);
  servo3.write(45);
  delay(tempoPausa);
  servo2.write(90);
  delay(tempoPausa);

  servo6.write(45);
  delay(tempoPausa);
  servo7.write(45);
  delay(tempoPausa);
  servo6.write(90);
  delay(tempoPausa);
  
  servo10.write(135);
  delay(tempoPausa);
  servo11.write(135);
  delay(tempoPausa);
  servo10.write(90);
  delay(tempoPausa);

  servo8.write(135);
  servo4.write(45);
  servo12.write(135);
  delay(tempoPausa);
  servo3.write(90);
  servo7.write(90);
  servo11.write(90);
  delay(tempoPausa);

  servo8.write(135);
  delay(tempoPausa);
  servo9.write(135);
  delay(tempoPausa);
  servo8.write(90);
  delay(tempoPausa);

  servo4.write(45);
  delay(tempoPausa);
  servo5.write(45);
  delay(tempoPausa);
  servo4.write(90);
  delay(tempoPausa);

  servo12.write(135);
  delay(tempoPausa);
  servo13.write(135);
  delay(tempoPausa);
  servo12.write(90);
  delay(tempoPausa);
  servo2.write(45);
  servo6.write(45);
  servo10.write(135);
  delay(tempoPausa);

  servo9.write(90);
  servo5.write(90);
  servo13.write(90);
  delay(tempoPausa);
}

void testa_bassa() {  
  delay(tempoPausa); 
  servo2.write(45);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
  servo6.write(90);
  servo7.write(90);
  servo8.write(135);
  servo9.write(90);
  servo10.write(90);
  servo11.write(90);
  servo12.write(90);
  servo13.write(90);
  delay(tempoPausa);

}

void gira_a_sinistra() {
  delay(tempoPausa);
  //zampa 1sx indietro
  servo2.write(45);
  delay(tempoPausa);
  servo3.write(45);
  delay(tempoPausa);
  servo2.write(90);
  delay(tempoPausa);
  //zampa 2sx indietro
  servo4.write(45);
  delay(tempoPausa);
  servo5.write(45);
  delay(tempoPausa);
  servo4.write(90);
  delay(tempoPausa);
  //zampa 3sx indietro
  servo6.write(45);
  delay(tempoPausa);
  servo7.write(45);
  delay(tempoPausa);
  servo6.write(90);
  delay(tempoPausa);
  //zampa 1dx avanti
  servo8.write(135);
  delay(tempoPausa);
  servo9.write(45);
  delay(tempoPausa);
  servo8.write(90);
  delay(tempoPausa);
  //zampa 2dx avanti
  servo10.write(135);
  delay(tempoPausa);
  servo11.write(45);
  delay(tempoPausa);
  servo10.write(90);
  delay(tempoPausa);
  //zampa 3dx avanti
  servo12.write(135);
  delay(tempoPausa);
  servo13.write(45);
  delay(tempoPausa);
  servo12.write(90);
  delay(tempoPausa);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
  servo6.write(90);
  servo7.write(90);
  servo8.write(90);
  servo9.write(90);
  servo10.write(90);
  servo11.write(90);
  servo12.write(90);
  servo13.write(90);
  delay(tempoPausa);

}

void gira_a_destra() {
  delay(tempoPausa);
  //zampa 1sx avanti
  servo2.write(45);
  delay(tempoPausa);
  servo3.write(135);
  delay(tempoPausa);
  servo2.write(90);
  delay(tempoPausa);
  //zampa 2sx avanti
  servo4.write(45);
  delay(tempoPausa);
  servo5.write(135);
  delay(tempoPausa);
  servo4.write(90);
  delay(tempoPausa);
  //zampa 3sx avanti
  servo6.write(45);
  delay(tempoPausa);
  servo7.write(135);
  delay(tempoPausa);
  servo6.write(90);
  delay(tempoPausa);
  //zampa 1dx indietro
  servo8.write(135);
  delay(tempoPausa);
  servo9.write(135);
  delay(tempoPausa);
  servo8.write(90);
  delay(tempoPausa);
  //zampa 2dx indietro
  servo10.write(135);
  delay(tempoPausa);
  servo11.write(135);
  delay(tempoPausa);
  servo10.write(90);
  delay(tempoPausa);
  //zampa 3dx indietro
  servo12.write(135);
  delay(tempoPausa);
  servo13.write(135);
  delay(tempoPausa);
  servo12.write(90);

  delay(tempoPausa);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
  servo6.write(90);
  servo7.write(90);
  servo8.write(90);
  servo9.write(90);
  servo10.write(90);
  servo11.write(90);
  servo12.write(90);
  servo13.write(90);
  delay(tempoPausa);
}

void testa_destra() {
  delay(tempoPausa);
  servo14.write(45);
  delay(tempoPausa);
}

void testa_sinistra() {
  delay(tempoPausa);
  servo14.write(135);
  delay(tempoPausa);
}

void testa_dritta() {
  delay(tempoPausa);
  servo14.write(90);
  delay(tempoPausa);
}