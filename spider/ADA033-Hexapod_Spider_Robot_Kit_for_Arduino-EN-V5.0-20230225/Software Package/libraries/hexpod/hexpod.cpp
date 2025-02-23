#include "hexpod.h"
#include "Arduino.h"

//add some libraries
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include<Wire.h>
#include <SR04.h>
#include "angle.h"

//define pins
#define PIN  A1
#define led_numbers  6
#define TRIG_PIN A2
#define ECHO_PIN A3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(led_numbers, PIN, NEO_GRB + NEO_KHZ800);
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);



//Declaring some global variables
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
long loop_timer;
int lcd_loop_counter;
float angle_pitch, angle_roll,angle_yaw;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output,angle_yaw_output;
float angle_changed;
long a;

// Create servo object
  Servo s0;
  Servo s1;
  Servo s2;
  Servo s3;
  Servo s4;
  Servo s5;
  Servo s6;
  Servo s7;
  Servo s8;
  Servo s9;
  Servo s10;
  Servo s11;
  Servo s12;

int i0H0 = 0;
int i1H0 = 0;
int i2H0 = 0;
int i3H0 = 0;
int i4H0 = 0;
int i5H0 = 0;
int i6H0 = 0;
int i7H0 = 0;
int i8H0 = 0;
int i9H0 = 0;
int i10H0 = 0;
int i11H0 = 0;
int i12H0 = 0;
  
int i0H1 = 0;
int i1H1 = 0;
int i2H1 = 0;
int i3H1 = 0;
int i4H1 = 0;
int i5H1 = 0;
int i6H1 = 0;
int i7H1 = 0;
int i8H1 = 0;
int i9H1 = 0;
int i10H1 = 0;
int i11H1 = 0;
int i12H1 = 0;

int i0H2 = 0;
int i1H2 = 0;
int i2H2 = 0;
int i3H2 = 0;
int i4H2 = 0;
int i5H2 = 0;
int i6H2 = 0;
int i7H2 = 0;
int i8H2 = 0;
int i9H2 = 0;
int i10H2 = 0;
int i11H2 = 0;
int i12H2 = 0;

int i0H3 = 0;
int i1H3 = 0;
int i2H3 = 0;
int i3H3 = 0;
int i4H3 = 0;
int i5H3 = 0;
int i6H3 = 0;
int i7H3 = 0;
int i8H3 = 0;
int i9H3 = 0;
int i10H3 = 0;
int i11H3 = 0;
int i12H3 = 0;

int i0H4 = 0;
int i1H4 = 0;
int i2H4 = 0;
int i3H4 = 0;
int i4H4 = 0;
int i5H4 = 0;
int i6H4 = 0;
int i7H4 = 0;
int i8H4 = 0;
int i9H4 = 0;
int i10H4 = 0;
int i11H4= 0;
int i12H4 = 0;

int i0H5 = 0;
int i1H5 = 0;
int i2H5 = 0;
int i3H5 = 0;
int i4H5 = 0;
int i5H5 = 0;
int i6H5 = 0;
int i7H5 = 0;
int i8H5 = 0;
int i9H5 = 0;
int i10H5 = 0;
int i11H5= 0;
int i12H5 = 0;

int status0 = 0;
int status1 = 0;
int status2 = 0;
int status3 = 0;
int status4 = 0;
int status5 = 0;

int battery_voltage;
double showbattery;


int actionspeed=1;
unsigned long showtime = 0;
char serialcom;
int angle=20;
char right=0;
char count = 0;
long counter=0;



 QUANRUPED::QUANRUPED()
 {
    ;
 }
 QUANRUPED::~QUANRUPED()
 {
   ;
 }
 
 
//This is the initialization angle of each leg, you can modify the following values according to your needs.
void original_latest(){
  s0.write(angle0);//D2
  s1.write(angle1);//D3
  s2.write(angle2);//D4
  s3.write(angle3);//D5
  s4.write(angle4);//D6
  s5.write(angle5); //D7
  s6.write(angle6);//D8
  s7.write(angle7); //D9
  s8.write(angle8);//D10
  s9.write(angle9); //D11
  s11.write(angle10);//D12
  s10.write(angle11);//D13
  s12.write(angle12);//D14
}


 
 
void QUANRUPED::servo_init()
{
  s0.write(angle0);//D2
  s1.write(angle1);//D3
  s2.write(angle2);//D4
  s3.write(angle3);//D5
  s4.write(angle4);//D6
  s5.write(angle5); //D7
  s6.write(angle6);//D8
  s7.write(angle7); //D9
  s8.write(angle8);//D10
  s9.write(angle9); //D11
  s11.write(angle10);//D12
  s10.write(angle11);//D13
  s12.write(angle12);//D14
}

void QUANRUPED::strip_begin(int r_data,int g_data,int b_data)
{
  for(int i=0;i<6;i++){
    strip.setPixelColor(i,strip.Color(r_data,g_data,b_data));
    strip.show();    
    delay(50);                                              
  } 
}
void QUANRUPED::servo_attach()
{
	 s0.attach(2);     s1.attach(3);     s2.attach(4);     s3.attach(5);     s4.attach(6);     s5.attach(7);     s6.attach(8);     s7.attach(9);     s8.attach(10);    s9.attach(11);    s10.attach(12);     s11.attach(13); s12.attach(14);
  s0.write(angle0); s1.write(angle1); s2.write(angle2); s3.write(angle3); s4.write(angle4); s5.write(angle5); s6.write(angle6); s7.write(angle7); s8.write(angle8); s9.write(angle9); s10.write(angle10); s11.write(angle11); s12.write(angle12); 
	strip.begin();strip.setBrightness(50);
	
}
void QUANRUPED::moveforward()
{
	for(int i=0;i<6;i++){
      strip.setPixelColor(i,strip.Color(255,0,0));
      strip.show();
    delay(1);                                                //Turn on the led if battery voltage is to low                                                    
    }                                                //Turn on the led if battery voltage is to low
	/******************step 1 to step 2************************/
 /**********rotate forward and rotate backward to the initial position**********/

//PROCESS 4:(raise the second leg)
while(status4<=45){
  status4++;
  s0.write(angle0);
//  s1.write(90);
//  delay(actionspeed);
  s2.write(angle2-i2H4);//100-i2H4
    i2H4++;
  if(i2H4==46 ){
    i2H4=0;
  }
//  s3.write(22.5);
delay(actionspeed);
s4.write(angle4);
//s5.write(120);
//delay(actionspeed);
  s6.write(angle6+i6H4);//90+i6H4
   i6H4++;
  if(i6H4==46){
      i6H4=0;
  }
//  s7.write(135);
//delay(1);
delay(actionspeed);

s8.write(angle8);
//s9.write(72.5);
//delay(actionspeed);
  s10.write(angle10+i10H4);//90+i10H4
    i10H4++;
    if(i10H4==46){
      i10H4=0;
  }
//  s11.write(90);
//delay(1);
delay(actionspeed);
//  s12.write(90+i12H4);
//strip.setBrightness(5+i12H4);
//strip.show();
////  
//  i12H4++;
//  if(i12H4==46){
//    i12H4=0;
//  }
//  if(i12H4<=20){i12H4++;}
//  else{i12H4=20;}
////  delay(1);
  if(status4>45){
  status4=0;
//  i12H4=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//PROCESS 3:
/*(rorate second leg forward move second leg back to the ground) and
(torate first leg backward)*/
while(status3<=45){
  status3++;
// Move back to touch the ground1
  s0.write(angle0);//90
  s1.write(angle1-i1H3);
  i1H3++;
  if(i1H3==46){
     i1H3=0;
  }
    
// delay(1);
  delay(actionspeed);
  // Rise the leg2
  s2.write(angle2-45+i2H3);
  i2H3++;
  if(i2H3==46){
     i2H3=0;
  }
  s3.write(angle3-45+i3H3);//45+i3H3
  i3H3++; 
  if(i3H3==46){
     i3H3=0;
  }
  
// delay(1);
delay(actionspeed);
// Move back to touch the ground3
  s4.write(angle4);  //90
  s5.write(angle5+30-i5H3);                              //165-i5H3
  i5H3++;
  if(i5H3==46){
     i5H3=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Rise the leg4
  s6.write(angle6+45-i6H3);
  i6H3++;
  if(i6H3==46){
      i6H3=0;
  }
  s7.write(angle7+45-i7H3);
  i7H3++;
  if(i7H3==46){
     i7H3=0;
  }
  
//delay(1);
delay(actionspeed);
// Move back to touch the ground5
  s8.write(angle8);  //90
  s9.write(angle9-17.5+i9H3);//50+i9H3
  i9H3++;
  if(i9H3==46){
     i9H3=0;
  }
  
//delay(1);
 delay(actionspeed);  

// Rise the leg6
  s10.write(angle10+45-i10H3);
  i10H3++;
  if(i10H3==46){
     i10H3=0;
  }
  s11.write(angle11-i11H3);                                          //45-i11H3
  i11H3++;
  if(i11H3==46){
    i11H3=0;
  }
  
//delay(1);
delay(actionspeed);
//  s12.write(110-i12H3);
//strip.setBrightness(50-i12H3);
//strip.show();
////  
//  i12H3++;
//  if(i12H3==46){
//    i12H3=0;
//  }
//  if(i12H3<=20){i12H3++;}
//  else{i12H3=20;}
////  delay(1);
  if(status3>45){
    status3 = 0;
//    i12H3=0;
    break;
    }
//    if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
  }

/*********rotate backward to the initial position and rotate forward**********/

//PROCESS 5:(raise the first leg)
while(status5<=45){
  status5++;
  s0.write(angle0-i0H5);//90-i0H5
  i0H5++;
  if(i0H5==46){
    i0H5=0;
  }
//  s1.write(45);
//  delay(actionspeed);
//delay(1);
s2.write(angle2);
//s3.write(67.5);
  delay(actionspeed);
  s4.write(angle4-i4H5);//90-i4H5
    i4H5++;
    if(i4H5==46){
      i4H5=0;
    }
//delay(1);
//s5.write(75);
//delay(actionspeed);
s6.write(angle6);
  
//  s7.write(90);
  delay(actionspeed);
  s8.write(angle8+i8H5);//90+i8H5
    i8H5++;
    if(i8H5==46){
      i8H5=0;
    }
//delay(1);
//s9.write(117.5);
//delay(actionspeed);
s10.write(angle10);
//s11.write(45);
delay(actionspeed);
//strip.setBrightness(5+i12H5);
//strip.show();
////    s12.write(90-i12H5);
//    i12H5++;
//  if(i12H5==46){
//    i12H5=0;
//  }
//  if(i12H5<=20){i12H5++;}
//  else{i12H5=20;}
////  delay(1);
  if(status5>45){
  status5=0;
//  i12H5=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}


//PROCESS 1:
/*(rorate first leg forward move first leg back to the ground) and
(rorate second leg backward)*/
while(status1<=45){
  status1++;
// Rise the leg1
  s0.write(angle0-45+i0H1);
  i0H1++;
  if(i0H1==46){
     i0H1=0;
  }
  s1.write(angle1-45+i1H1); 
  i1H1++;
  if(i1H1==46){
    i1H1=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Move back to touch the ground2       
  s2.write(angle2); 
  s3.write(angle3-i3H1);//90-i3H1
  i3H1++;
  if(i3H1==46){
    i3H1=0;                       
  }
                        
//delay(1);
  delay(actionspeed);
// Rise the leg3
  s4.write(angle4-45+i4H1);
  i4H1++;
  if(i4H1==46){
     i4H1=0;
  }
  s5.write(angle5-15+i5H1);//120+i5H1
  i5H1++;
  if(i5H1==46){
     i5H1=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Move back to touch the ground4
  s6.write(angle6); 
  s7.write(angle7+i7H1);
  i7H1++;
  if(i7H1==46){
     i7H1=0;
  }
   
//delay(1);
  delay(actionspeed);
// Rise the leg5
  s8.write(angle8+45-i8H1);
  i8H1++;
  if(i8H1==46){
     i8H1=0;
  }
  s9.write(angle9+27.5-i9H1);//95-i9H1
  i9H1++;
  if(i9H1==46){
     i9H1=0;
  }
  
//delay(1);
 delay(actionspeed);
  
// Move back to touch the ground6
  s10.write(angle10);
  s11.write(angle11-45+i11H1);//0+i11H1
  i11H1++;
  if(i11H1==46){
     i11H1=0;
  }
    
//delay(1);
delay(actionspeed);
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
//  if(i12H1<=20){i12H1++;}
//  else{i12H1=20;}
////  delay(1);
if(status1>45){
  status1 = 0;
//  i12H1=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//ultrasonic();
//if(a<=20){
//  while(right<5){
//    right++;
//    move_right_latest1();
//    if(Serial.available()>0){
//    break;
//    }
//  }
//  right=0;
//  }
//  battery();

//  sendbattery();

}
void QUANRUPED::movebackward()
{
	for(int i=0;i<6;i++){
      strip.setPixelColor(i,strip.Color(55,122,132));
      strip.show(); 
      delay(1);                                              
     }
  /******************step 1 to step 2************************/
  // Rotate the leg1
 //PROCESS 5:
  
while(status5<=45){
  status5++;
  s0.write(angle0-i0H5);
  i0H5++;
  if(i0H5==46){
    i0H5=0;
  }
//  s1.write(90);
//  delay(actionspeed);
//    delay(1);
s2.write(angle2);
//s3.write(22.5);
  delay(actionspeed);
  s4.write(angle4-i4H5);
    i4H5++;
    if(i4H5==46){
      i4H5=0;
    }
//    delay(1);
//s5.write(120);
//delay(actionspeed);
s6.write(angle6);

//     s7.write(135);
     delay(actionspeed);
  s8.write(angle8+i8H5);
    i8H5++;
    if(i8H5==46){
      i8H5=0;
    }
//  delay(1); 
//s9.write(72.5);
//delay(actionspeed);
s10.write(angle10);
//s11.write(90);
  delay(actionspeed);
//  strip.setBrightness(5+i12H1);
//  strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status5>45){
  status5=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//PROCESS 3:
while(status3<=45){
  status3++;
// Move back to touch the ground1
  s0.write(angle0-45+i0H3);
  i0H3++;
  if(i0H3==46){
      i0H3=0;
  }
  s1.write(angle1-i1H3);
  i1H3++;
  if(i1H3==46){
     i1H3=0;
  }
  
//    delay(1); 
delay(actionspeed);
  
  // Rise the leg2
  s2.write(angle2);
  s3.write(angle3-45+i3H3);//45+i3H3
  i3H3++; 
  if(i3H3==46){
     i3H3=0;
  }
  
//  delay(1); 

delay(actionspeed);
  
// Move back to touch the ground3
  s4.write(angle4-45+i4H3); 
  i4H3++;
  if(i4H3==46){
     i4H3=0;
  }
  s5.write(angle5+30-i5H3);//165-i5H3
  i5H3++;
  if(i5H3==46){
     i5H3=0;
  }
  
//  delay(1); 
delay(actionspeed);
 
  // Rise the leg4
  s6.write(angle6);
  s7.write(angle7+45-i7H3);
  i7H3++;
  if(i7H3==46){
     i7H3=0;
  }
  
//  delay(1); 
delay(actionspeed);

// Move back to touch the ground5
  s8.write(angle8+45-i8H3);
  i8H3++;
  if(i8H3==46){
      i8H3=0;
  }
  s9.write(angle9-17.5+i9H3);//50+i9H3
  i9H3++;
  if(i9H3==46){
     i9H3=0;
  }
  
//    delay(1); 

   delay(actionspeed);

// Rise the leg6
  s10.write(angle10);
  s11.write(angle11-i11H3);//45-i11H3
  i11H3++;
  if(i11H3==46){
    i11H3=0;
  }
  
  
  
//  delay(1); 
delay(actionspeed);
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status3>45){
    status3 = 0;
    break;
    }
//    if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
  }

/*********rotate backward to the initial position and rotate forward**********/

//PROCESS 4:
while(status4<=45){
  status4++;
  s0.write(angle0);
//  s1.write(45);
//  delay(actionspeed);
  s2.write(angle2-i2H4);
  i2H4++;
  if(i2H4==46){
    i2H4=0;
  }
//  s3.write(67.5);
//   delay(1); 

delay(actionspeed);
s4.write(angle4);
//s5.write(75);
//delay(actionspeed);
  s6.write(angle6+i6H4);
    i6H4++;
  if(i6H4==46){
      i6H4=0;
  }
//    delay(1); 
//s7.write(90);
delay(actionspeed);

s8.write(angle8);
//s9.write(117.5);
//delay(actionspeed);
  s10.write(angle10+i10H4);
    i10H4++;
    if(i10H4==46){
      i10H4=0;
  }
//  s11.write(45);
//  delay(1); 
delay(actionspeed);

//strip.setBrightness(5+i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status4>45){
  status4=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//PROCESS 1:
while(status1<=45){
  status1++;
// Rise the leg1
  s0.write(angle0);
  s1.write(angle1-45+i1H1); 
  i1H1++;
  if(i1H1==46){
    i1H1=0;
  }
  
//  delay(1); 
delay(actionspeed);
  
  // Move back to touch the ground2       
  s2.write(angle2-45+i2H1);
  i2H1++;
  if(i2H1==46){
    i2H1=0;                           
  }
  s3.write(angle3-i3H1);//90-i3H1
  i3H1++;
  if(i3H1==46){
    i3H1=0;                       
  }
  
//   delay(1);
delay(actionspeed);
  
// Rise the leg3
  s4.write(angle4);
  s5.write(angle5-15+i5H1);//120+i5H1
  i5H1++;
  if(i5H1==46){
     i5H1=0;
  }
  
//  delay(1); 
delay(actionspeed);
  
  // Move back to touch the ground4
  s6.write(angle6+45-i6H1);
  i6H1++;
  if(i6H1==46){
      i6H1=0;
  }
  s7.write(angle7+i7H1);
  i7H1++;
  if(i7H1==46){
     i7H1=0;
  }
  
//  delay(1); 

  delay(actionspeed);
// Rise the leg5
  s8.write(angle8);
  s9.write(angle9+27.5-i9H1);//95-i9H1
  i9H1++;
  if(i9H1==46){
     i9H1=0;
  }
  
  
//   delay(1); 
delay(actionspeed);
  
// Move back to touch the ground6
  s10.write(angle10+45-i10H1);
  i10H1++;
  if(i10H1==46){
      i10H1=0;
  }
  s11.write(angle11-45+i11H1);//0+i11H1
  i11H1++;
  if(i11H1==46){
     i11H1=0;
  }
  
//  delay(1);  
delay(actionspeed);
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  
if(status1>45){
  status1 = 0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}
//battery();

//sendbattery();

/**********rotate forward and rotate backward to the initial position**********/
	
}

void QUANRUPED::turnright()
{
	for(int i=0;i<6;i++){
      strip.setPixelColor(i,strip.Color(122,55,232));
      strip.show(); 
      delay(1);                                              
     }
/********************step 1  first******************************/  
 //   s0.write(90); 
//   s4.write(90);
//   s8.write(90);
//PROCESS 4:(raise the second leg)
while(status4<=45){
  status4++;
  s0.write(angle0);
//  s1.write(90);
//  delay(actionspeed);
  s2.write(angle2-i2H4);
  i2H4++;
  if(i2H4==46){
    i2H4=0;
  }
//  delay(1);
//s3.write(22.5);
delay(actionspeed);
s4.write(angle4);

//s5.write(120);
//delay(actionspeed);
  s6.write(angle6+i6H4);
    i6H4++;
  if(i6H4==46){
      i6H4=0;
  }
//  delay(1);
//s7.write(90);
delay(actionspeed);

s8.write(angle8);
//s9.write(117.5);
//delay(actionspeed);

  s10.write(angle10+i10H4);
    i10H4++;
    if(i10H4==46){
      i10H4=0;
  }
//  delay(1);
//s11.write(45);
delay(actionspeed);
//strip.setBrightness(5+i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status4>45){
  status4=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//PROCESS 3:
/*(rorate second leg forward move second leg back to the ground) and
(torate first leg backward)*/
while(status3<=45){
  status3++;
// Move back to touch the ground1
  s0.write(angle0);
  s1.write(angle1-i1H3);
  i1H3++;
  if(i1H3==46){
     i1H3=0;
  }
    
//  delay(1);   
  delay(actionspeed);
  // Rise the leg2
  s2.write(angle2-45+i2H3);
  i2H3++;
  if(i2H3==46){
     i2H3=0;
  }
  s3.write(angle3-45+i3H3);//45+i3H3
  i3H3++; 
  if(i3H3==46){
     i3H3=0;
  }
  delay(actionspeed);
//  delay(1); 
  
// Move back to touch the ground3
  s4.write(angle4);
  s5.write(angle5+30-i5H3);//165-i5H3
  i5H3++;
  if(i5H3==46){
     i5H3=0;
  }
    
//  delay(1); 
  delay(actionspeed);
  // Rise the leg4

  s6.write(angle6+45-i6H3);
  i6H3++;
  if(i6H3==46){
     i6H3=0;
  }  
  s7.write(angle7+i7H3);
  i7H3++;
  if(i7H3==46){
     i7H3=0;
  }
  delay(actionspeed);
//  delay(1);  
// Move back to touch the ground5
  s8.write(angle8);
  s9.write(angle9+27.5-i9H3);//95-i9H3
  i9H3++;
  if(i9H3==46){
     i9H3=0;
  }
    
//  delay(1);    
   delay(actionspeed);

// Rise the leg6
  s10.write(angle10+45-i10H3);
  i10H3++;
  if(i10H3==46){
     i10H3=0;
  }
  s11.write(angle11-45+i11H3);//0+i11H3
  i11H3++;
  if(i11H3==46){
    i11H3=0;
  }
  delay(actionspeed);
//  delay(1);
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status3>45){
    status3 = 0;
    break;
    }
//    if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
  }

/*********rotate backward to the initial position and rotate forward**********/


//PROCESS 5:(raise the first leg)
while(status5<=45){
  status5++;
  s0.write(angle0-i0H5);
  
  i0H5++;
  if(i0H5==46){
    i0H5=0;
  }
//  s1.write(45);
//  delay(actionspeed);
  s2.write(angle2);
//  s3.write(67.5);
//  delay(1);
  delay(actionspeed);
  s4.write(angle4-i4H5);
    i4H5++;
    if(i4H5==46){
      i4H5=0;
    }
//  delay(1); 
//s5.write(75);
//delay(actionspeed);
s6.write(angle6);
 
//  s7.write(135);
  delay(actionspeed);
  s8.write(angle8+i8H5);
    i8H5++;
    if(i8H5==46){
      i8H5=0;
    }
//    s9.write(72.5);
//    delay(actionspeed);
    s10.write(angle10);
//  delay(1);
//s11.write(90);
delay(actionspeed);
//strip.setBrightness(5+i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status5>45){
  status5=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}


//PROCESS 1:
/*(rorate first leg forward move first leg back to the ground) and
(torate second leg backward)*/
while(status1<=45){
  status1++;
// Rise the leg1
  s0.write(angle0-45+i0H1);
  i0H1++;
  if(i0H1==46){
     i0H1=0;
  }
  s1.write(angle1-45+i1H1); 
  i1H1++;
  if(i1H1==46){
    i1H1=0;
  }
  delay(actionspeed);
//  delay(1); 
  
  // Move back to touch the ground2       
   s2.write(angle2); 
  s3.write(angle3-i3H1);//90-i3H1
  i3H1++;
  if(i3H1==46){
    i3H1=0;                       
  }
   delay(actionspeed);                     
//  delay(1); 
  
// Rise the leg3
  s4.write(angle4-45+i4H1);
  i4H1++;
  if(i4H1==46){
     i4H1=0;
  }
  s5.write(angle5-15+i5H1);//120+i5H1
  i5H1++;
  if(i5H1==46){
     i5H1=0;
  }
  delay(actionspeed);
//  delay(1);
  
  // Move back to touch the ground4
    s6.write(angle6);
  s7.write(angle7+45-i7H1);
  i7H1++;
  if(i7H1==46){
     i7H1=0;
  }
   delay(actionspeed);
//  delay(1); 
  
// Rise the leg5
  s9.write(angle9-17.5+i9H1);//50+i9H1
  i9H1++;
  if(i9H1==46){
     i9H1=0;
  }
  s8.write(angle8+45-i8H1);
  i8H1++;
  if(i8H1==46){
     i8H1=0;
  }
//  delay(1);  
 delay(actionspeed);
  
// Move back to touch the ground6
  s10.write(angle10);
  s11.write(angle11-i11H1);//45-i11H1
  i11H1++;
  if(i11H1==46){
     i11H1=0;
  }
    delay(actionspeed);
//  delay(1);  
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
if(status1>45){
  status1 = 0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//battery();

//sendbattery();


/**********rotate forward and rotate backward to the initial position**********/

	
}
void QUANRUPED::turnleft()
{
	 for(int i=0;i<6;i++){
      strip.setPixelColor(i,strip.Color(122,232,55));
      strip.show(); 
      delay(1);                                              
     }
/********************step 1  third******************************/  
 //PROCESS 4:(raise the second leg)
while(status4<=45){
  status4++;
  s0.write(angle0);
//  s1.write(45);
//  delay(actionspeed);
  s2.write(angle2-i2H4);
  i2H4++;
  if(i2H4==46){
    i2H4=0;
  }
//  s3.write(67.5);
//  delay(1);
delay(actionspeed);
s4.write(angle4);
//s5.write(75);
//delay(actionspeed);
  s6.write(angle6+i6H4);
    i6H4++;
  if(i6H4==46){
      i6H4=0;
  }
//  s7.write(135);
//  delay(1);
delay(actionspeed);
s8.write(angle8);
//s9.write(72.5);
//delay(actionspeed);
  s10.write(angle10+i10H4);
    i10H4++;
    if(i10H4==46){
      i10H4=0;
  }
//  delay(1);
//s11.write(90);
delay(actionspeed);
//strip.setBrightness(5+i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status4>45){
  status4=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}

//PROCESS 3:
/*(rorate second leg forward move second leg back to the ground) and
(torate first leg backward)*/
while(status3<=45){
  status3++;
// Move back to touch the ground1
  s0.write(angle0);
  s1.write(angle1-45+i1H3);
  i1H3++;
  if(i1H3==46){
     i1H3=0;
  }
    
//  delay(1);   
  delay(actionspeed);
  // Rise the leg2
  s2.write(angle2-45+i2H3);
  i2H3++;
  if(i2H3==46){
     i2H3=0;
  }
  s3.write(angle3-i3H3);//90-i3H3
  i3H3++; 
  if(i3H3==46){
     i3H3=0;
  }
  delay(actionspeed);
//  delay(1); 
  
// Move back to touch the ground3
  s4.write(angle4);
  s5.write(angle5-15+i5H3);//120+i5H3
  i5H3++;
  if(i5H3==46){
     i5H3=0;
  }
    
//  delay(1); 
  delay(actionspeed);
  // Rise the leg4

  s6.write(angle6+45-i6H3);
  i6H3++;
  if(i6H3==46){
     i6H3=0;
  }  
  s7.write(angle7+45-i7H3);
  i7H3++;
  if(i7H3==46){
     i7H3=0;
  }
//  delay(1); 
delay(actionspeed); 
// Move back to touch the ground5
  s8.write(angle8);
  s9.write(angle9-17.5+i9H3);//50+i9H3
  i9H3++;
  if(i9H3==46){
     i9H3=0;
  }
    
//  delay(1);    
   delay(actionspeed);

// Rise the leg6
  s10.write(angle10+45-i10H3);
  i10H3++;
  if(i10H3==46){
     i10H3=0;
  }
  s11.write(angle11-i11H3);//45-i11H3
  i11H3++;
  if(i11H3==46){
    i11H3=0;
  }
  delay(actionspeed);
//  delay(1);
//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status3>45){
    status3 = 0;
    break;
    }
//    if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
  }

/*********rotate backward to the initial position and rotate forward**********/


//PROCESS 5:(raise the first leg)
while(status5<=45){
  status5++;
  s0.write(angle0-i0H5);
  i0H5++;
  if(i0H5==46){
    i0H5=0;
  }
//  s1.write(90);
//  delay(actionspeed);
//  delay(1);
s2.write(angle2);
  
//  s3.write(22.5);
  delay(actionspeed);
  s4.write(angle4-i4H5);
    i4H5++;
    if(i4H5==46){
      i4H5=0;
    }
//  delay(1); 
//s5.write(120);
//delay(actionspeed);
s6.write(angle6);
  
//  s7.write(90);
  delay(actionspeed);
  s8.write(angle8+i8H5);
    i8H5++;
    if(i8H5==46){
      i8H5=0;
    }
//  delay(1);
//s9.write(117.5);
//delay(actionspeed);
s10.write(angle10);
//s11.write(45);
  delay(actionspeed);

//  strip.setBrightness(5+i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(status5>45){
  status5=0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}


//PROCESS 1:
/*(rorate first leg forward move first leg back to the ground) and
(torate second leg backward)*/
while(status1<=45){
  status1++;
// Rise the leg1
  s0.write(angle0-45+i0H1);
  i0H1++;
  if(i0H1==46){
     i0H1=0;
  }
  s1.write(angle1-i1H1); 
  i1H1++;
  if(i1H1==46){
    i1H1=0;
  }
  delay(actionspeed);
//  delay(1); 
  
  // Move back to touch the ground2       
  s2.write(angle2); 
  s3.write(angle3-45+i3H1);//45+i3H1
  i3H1++;
  if(i3H1==46){
    i3H1=0;                       
  }
   delay(actionspeed);               
//  delay(1); 
  
// Rise the leg3
  s4.write(angle4-45+i4H1);
  i4H1++;
  if(i4H1==46){
     i4H1=0;
  }
  s5.write(angle5+30-i5H1);//165-i5H1
  i5H1++;
  if(i5H1==46){
     i5H1=0;
  }
  delay(actionspeed);
//  delay(1);
  
  // Move back to touch the ground4
  s6.write(angle6);
  s7.write(angle7+i7H1);
  i7H1++;
  if(i7H1==46){
     i7H1=0;
  }
   delay(actionspeed);
//  delay(1); 
  
// Rise the leg5
  s8.write(angle8+45-i8H1);
  i8H1++;
  if(i8H1==46){
     i8H1=0;
  }
  s9.write(angle9+27.5-i9H1);//95-i9H1
  i9H1++;
  if(i9H1==46){
     i9H1=0;
  }
  
//  delay(1);  
 delay(actionspeed);
  
// Move back to touch the ground6
  s10.write(angle10);
  s11.write(angle11-45+i11H1);//0+i11H1
  i11H1++;
  if(i11H1==46){
     i11H1=0;
  }
  delay(actionspeed);  
//  delay(1);  

//strip.setBrightness(50-i12H1);
//strip.show();
////  s12.write(70+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  
if(status1>45){
  status1 = 0;
  break;
  }
//  if(Serial.available()>0){
//    break;
//  }
//  delay(actionspeed);
}
//battery();

//sendbattery();

/**********rotate forward and rotate backward to the initial position**********/
	
}

void QUANRUPED::advoid()
{
	
/**********rotate forward and rotate backward to the initial position**********/
//  char R=random(0,255);

//PROCESS 4:(raise the second leg)
while(status4<=45){
  status4++;
  s0.write(angle0);
//  s1.write(90);
//  delay(actionspeed);
  s2.write(angle2+10-i2H4);//100-i2H4
    i2H4++;
  if(i2H4==46 ){
    i2H4=0;
  }
//  s3.write(22.5);
delay(actionspeed);
s4.write(angle4);
//s5.write(120);
//delay(actionspeed);
  s6.write(angle6+i6H4);//90+i6H4
   i6H4++;
  if(i6H4==46){
      i6H4=0;
  }
//  s7.write(135);
//delay(1);
delay(actionspeed);

s8.write(angle8);
//s9.write(72.5);
//delay(actionspeed);
  s10.write(angle10+i10H4);//90+i10H4
    i10H4++;
    if(i10H4==46){
      i10H4=0;
  }
//  s11.write(90);
//delay(1);
delay(actionspeed);
  s12.write(angle12+i12H4);
  
//  i12H4++;
//  if(i12H4==46){
//    i12H4=0;
//  }
  if(i12H4<=20){i12H4++;}
  else{i12H4=20;}
//  delay(1);
  if(status4>45){
  status4=0;
  i12H4=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
//  delay(actionspeed);
}
//  char G=random(0,255);
  
//PROCESS 3:
/*(rorate second leg forward move second leg back to the ground) and
(torate first leg backward)*/
while(status3<=45){
  status3++;
// Move back to touch the ground1
  s0.write(angle0);//90
  s1.write(angle1-i1H3);
  i1H3++;
  if(i1H3==46){
     i1H3=0;
  }
    
// delay(1);
  delay(actionspeed);
  // Rise the leg2
  s2.write(angle2-45+i2H3);
  i2H3++;
  if(i2H3==46){
     i2H3=0;
  }
  s3.write(angle3-67.5+i3H3);//45+i3H3
  i3H3++; 
  if(i3H3==46){
     i3H3=0;
  }
  
// delay(1);
delay(actionspeed);
// Move back to touch the ground3
  s4.write(angle4);  //90
  s5.write(angle5+30-i5H3);                              //165-i5H3
  i5H3++;
  if(i5H3==46){
     i5H3=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Rise the leg4
  s6.write(angle6+45-i6H3);
  i6H3++;
  if(i6H3==46){
      i6H3=0;
  }
  s7.write(angle7+45-i7H3);
  i7H3++;
  if(i7H3==46){
     i7H3=0;
  }
  
//delay(1);
delay(actionspeed);
// Move back to touch the ground5
  s8.write(angle8);  //90
  s9.write(angle9-17.5+i9H3);//50+i9H3
  i9H3++;
  if(i9H3==46){
     i9H3=0;
  }
  
//delay(1);
 delay(actionspeed);  

// Rise the leg6
  s10.write(angle10+45-i10H3);
  i10H3++;
  if(i10H3==46){
     i10H3=0;
  }
  s11.write(angle11-i11H3);                                          //45-i11H3
  i11H3++;
  if(i11H3==46){
    i11H3=0;
  }
  
//delay(1);
delay(actionspeed);
  s12.write(angle12+20-i12H3);
  
//  i12H3++;
//  if(i12H3==46){
//    i12H3=0;
//  }
  if(i12H3<=20){i12H3++;}
  else{i12H3=20;}
//  delay(1);
  if(status3>45){
    status3 = 0;
    i12H3=0;
    break;
    }
    if(Serial.available()>0){
    break;
  }
//  delay(actionspeed);
  }
//char B=random(0,255);
/*********rotate backward to the initial position and rotate forward**********/

//PROCESS 5:(raise the first leg)
while(status5<=45){
  status5++;
  s0.write(angle0-i0H5);//90-i0H5
  i0H5++;
  if(i0H5==46){
    i0H5=0;
  }
//  s1.write(45);
//  delay(actionspeed);
//delay(1);
s2.write(angle2);
//s3.write(67.5);
  delay(actionspeed);
  s4.write(angle4-i4H5);//90-i4H5
    i4H5++;
    if(i4H5==46){
      i4H5=0;
    }
//delay(1);
//s5.write(75);
//delay(actionspeed);
s6.write(angle6);
  
//  s7.write(90);
  delay(actionspeed);
  s8.write(angle8+i8H5);//90+i8H5
    i8H5++;
    if(i8H5==46){
      i8H5=0;
    }
//delay(1);
//s9.write(117.5);
//delay(actionspeed);
s10.write(angle10);
//s11.write(45);
delay(actionspeed);
    s12.write(angle12-i12H5);
//    i12H5++;
//  if(i12H5==46){
//    i12H5=0;
//  }
  if(i12H5<=20){i12H5++;}
  else{i12H5=20;}
//  delay(1);
  if(status5>45){
  status5=0;
  i12H5=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
//  delay(actionspeed);
}

for(int i=0;i<6;i++){
      strip.setPixelColor(i,strip.Color(129,67,213));
      strip.show();    
      delay(1);                                              
    } 
//PROCESS 1:
/*(rorate first leg forward move first leg back to the ground) and
(rorate second leg backward)*/
while(status1<=45){
  status1++;
// Rise the leg1
  s0.write(angle0-45+i0H1);
  i0H1++;
  if(i0H1==46){
     i0H1=0;
  }
  s1.write(angle1-45+i1H1); 
  i1H1++;
  if(i1H1==46){
    i1H1=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Move back to touch the ground2       
  s2.write(angle2); 
  s3.write(angle3-22.5-i3H1);//90-i3H1
  i3H1++;
  if(i3H1==46){
    i3H1=0;                       
  }
                        
//delay(1);
  delay(actionspeed);
// Rise the leg3
  s4.write(angle4-45+i4H1);
  i4H1++;
  if(i4H1==46){
     i4H1=0;
  }
  s5.write(angle5-25+i5H1);//120+i5H1
  i5H1++;
  if(i5H1==46){
     i5H1=0;
  }
  
//delay(1);
  delay(actionspeed);
  // Move back to touch the ground4
  s6.write(angle6); 
  s7.write(angle7+i7H1);
  i7H1++;
  if(i7H1==46){
     i7H1=0;
  }
   
//delay(1);
  delay(actionspeed);
// Rise the leg5
  s8.write(angle8+45-i8H1);
  i8H1++;
  if(i8H1==46){
     i8H1=0;
  }
  s9.write(angle9+27.5-i9H1);//95-i9H1
  i9H1++;
  if(i9H1==46){
     i9H1=0;
  }
  
//delay(1);
 delay(actionspeed);
  
// Move back to touch the ground6
  s10.write(angle10);
  s11.write(angle11-45+i11H1);//0+i11H1
  i11H1++;
  if(i11H1==46){
     i11H1=0;
  }
    
//delay(1);
delay(actionspeed);
  s12.write(angle12-20+i12H1);
//  i12H1++;
//  if(i12H1==46){
//    i12H1=0;
//  }
  if(i12H1<=20){i12H1++;}
  else{i12H1=20;}
//  delay(1);
if(status1>45){
  status1 = 0;
  i12H1=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
//  delay(actionspeed);
}

ultrasonic();
if(a<=20 && a>0){
  while(right<6){
    right++;
    turnright();
    if(Serial.available()>0){
    break;
    }
  }
  right=0;
  }
//  battery();

//  sendbattery();

}



void QUANRUPED::ultrasonic()
{
	a=sr04.Distance();
  if(a>100){
    a=0;
  }
  
//  if(angel%3 == 0){
//  u[]=a;
//  }
  
//  Serial.print(a);
//  Serial.println("cm");
}

void step_by_step(){
//  while(status1<=45){
//    status1++;
//    s0.write(90+i0H0);
//    i0H0++;
//    if(i0H0==46){
//      i0H0=0;
//    }
//    s2.write(90+i2H0);
//    i2H0++;
//    if(i2H0==46){
//      i2H0=0;
//    }
//    s4.write(90+i4H0);
//    i4H0++;
//    if(i4H0==46){
//      i4H0=0
//    }
//    s6.write(90+i6H0);
//    i6H0++;
//    if(i6H0==46){
//      i6H0=0;
//    }
//    s8.write(90+i8H0);
//    i8H0++;
//    if(i8H0==46){
//      i8H0=0;
//    }
//    s10.write(90+i10H0);
//    i10H0++;
//    if(i10H0==46){
//      i10H0=0;
//    }
//    s1.write(45);
//    s3.write(45);
//    s5.write(120);
//    s7.write(135);
//    s9.write(95);
//    s11.write(45);
//    if(status1>45){
//      status1=0;
//      break;
//    }
//  }


  while(status2<=45){
    status2++;
    s0.write(angle0+45-i0H0);
    i0H0++;
    if(i0H0==46){
      i0H0=0;
    }
    delay(actionspeed);
    s2.write(angle2+45-i2H0);
    i2H0++;
    if(i2H0==46){
      i2H0=0;
    }
    delay(actionspeed);
    s4.write(angle4+45-i4H0);
    i4H0++;
    if(i4H0==46){
      i4H0=0;
    }
    delay(actionspeed);
    s6.write(angle6+45-i6H0);
    i6H0++;
    if(i6H0==46){
      i6H0=0;
    }
    delay(actionspeed);
    s8.write(angle8+45-i8H0);
    i8H0++;
    if(i8H0==46){
      i8H0=0;
    }
    delay(actionspeed);
    s10.write(angle10+45-i10H0);
    i10H0++;
    if(i10H0==46){
      i10H0=0;
    }
    delay(actionspeed);
    s1.write(angle1-45-i1H0);
    i1H0++;
    if(i1H0==46){
      i1H0=0;
    }
    delay(actionspeed);
    s3.write(angle3-45-i3H0);
    i3H0++;
    if(i3H0==46){
      i3H0=0;
    }
    delay(actionspeed);
    s5.write(angle5+30-i5H0);
    i5H0++;
    if(i5H0==46){
      i5H0=0;
    }
    delay(actionspeed);
    s7.write(angle7+45+i7H0);
    i7H0++;
    if(i7H0==46){
      i7H0=0;
    }
    delay(actionspeed);
    s9.write(angle9+5+i9H0);
    i9H0++;
    if(i9H0==46){
      i9H0=0;
    }
    delay(actionspeed);
    s11.write(angle11-45+i11H0);
    i11H0++;
    if(i11H0==46){
      i11H0=0;
    }
    delay(actionspeed);
    if(status2>45){
      status2=0;
      break;
    }
    if(Serial.available()>0){
      break;
    }
    delay(actionspeed);
  }


  while(status3<=45){
    status3++;
    s0.write(angle0-i0H0);
    i0H0++;
    if(i0H0==46){
      i0H0=0;
    }
    delay(actionspeed);
    s2.write(angle2-i2H0);
    i2H0++;
    if(i2H0==46){
      i2H0=0;
    }
    delay(actionspeed);
    s4.write(angle4-i4H0);
    i4H0++;
    if(i4H0==46){
      i4H0=0;
    }
    delay(actionspeed);
    s6.write(angle6-i6H0);
    i6H0++;
    if(i6H0==46){
      i6H0=0;
    }
    delay(actionspeed);
    s8.write(angle8-i8H0);
    i8H0++;
    if(i8H0==46){
      i8H0=0;
    }
    delay(actionspeed);
    s10.write(angle10-i10H0);
    i10H0++;
    if(i10H0==46){
      i10H0=0;
    }
    delay(actionspeed);
    s1.write(0+i1H0);
    i1H0++;
    if(i1H0==46){
      i1H0=0;
    }
    delay(actionspeed);
    s3.write(0+i3H0);
    i3H0++;
    if(i3H0==46){
      i3H0=0;
    }
    delay(actionspeed);
    s5.write(angle5-15+i5H0);
    i5H0++;
    if(i5H0==46){
      i5H0=0;
    }
    delay(actionspeed);
    s7.write(180-i7H0);
    i7H0++;
    if(i7H0==46){
      i7H0=0;
    }
    delay(actionspeed);
    s9.write(angle9+45-i9H0);
    i9H0++;
    if(i9H0==46){
      i9H0=0;
    }
    delay(actionspeed);
    s11.write(angle11-i11H0);
    i11H0++;
    if(i11H0==46){
      i11H0=0;
    }
    delay(actionspeed);
    if(status3>45){
      status3=0;
      break;
    }
    if(Serial.available()>0){
      break;
    }
    delay(actionspeed);
  }

  while(status4<=45){
    status4++;
    s0.write(angle0-45+i0H0);
    i0H0++;
    if(i0H0==46){
      i0H0=0;
    }
    delay(actionspeed);
    s2.write(angle2-45+i2H0);
    i2H0++;
    if(i2H0==46){
      i2H0=0;
    }
    delay(actionspeed);
    s4.write(angle4-45+i4H0);
    i4H0++;
    if(i4H0==46){
      i4H0=0;
    }
    delay(actionspeed);
    s6.write(angle6-45+i6H0);
    i6H0++;
    if(i6H0==46){
      i6H0=0;
    }
    delay(actionspeed);
    s8.write(angle8-45+i8H0);
    i8H0++;
    if(i8H0==46){
      i8H0=0;
    }
    delay(actionspeed);
    s10.write(angle10-45+i10H0);
    i10H0++;
    if(i10H0==46){
      i10H0=0;
    }
    delay(actionspeed);
    s1.write(angle1-45+i1H0);
    i1H0++;
    if(i1H0==46){
      i1H0=0;
    }
    delay(actionspeed);
    s3.write(angle3-45+i3H0);
    i3H0++;
    if(i3H0==46){
      i3H0=0;
    }
    delay(actionspeed);
    s5.write(angle5+30+i5H0);
    i5H0++;
    if(i5H0==46){
      i5H0=0;
    }
    delay(actionspeed);
    s7.write(angle7+45-i7H0);
    i7H0++;
    if(i7H0==46){
      i7H0=0;
    }
    delay(actionspeed);
    s9.write(angle9+5-i9H0);
    i9H0++;
    if(i9H0==46){
      i9H0=0;
    }
    delay(actionspeed);
    s11.write(angle11-45-i11H0);
    i11H0++;
    if(i11H0==46){
      i11H0=0;
    }
    delay(actionspeed);
    if(status4>45){
      status4=0;
      break;
    }
    if(Serial.available()>0){
      break;
    }
    delay(actionspeed);
  }

  while(status5<=45){
    status5++;
    s0.write(angle0+i0H0);
    i0H0++;
    if(i0H0==46){
      i0H0=0;
    }
    delay(actionspeed);
    s2.write(angle2+i2H0);
    i2H0++;
    if(i2H0==46){
      i2H0=0;
    }
    delay(actionspeed);
    s4.write(angle4+i4H0);
    i4H0++;
    if(i4H0==46){
      i4H0=0;
    }
    delay(actionspeed);
    s6.write(angle6+i6H0);
    i6H0++;
    if(i6H0==46){
      i6H0=0;
    }
    delay(actionspeed);
    s8.write(angle8+i8H0);
    i8H0++;
    if(i8H0==46){
      i8H0=0;
    }
    delay(actionspeed);
    s10.write(angle10+i10H0);
    i10H0++;
    if(i10H0==46){
      i10H0=0;
    }
    delay(actionspeed);
    s1.write(angle1-i1H0);
    i1H0++;
    if(i1H0==46){
      i1H0=0;
    }
    delay(actionspeed);
    s3.write(angle3-i3H0);
    i3H0++;
    if(i3H0==46){
      i3H0=0;
    }
    delay(actionspeed);
    s5.write(angle5+75-i5H0);
    i5H0++;
    if(i5H0==46){
      i5H0=0;
    }
    delay(actionspeed);
    s7.write(angle7+i7H0);
    i7H0++;
    if(i7H0==46){
      i7H0=0;
    }
    delay(actionspeed);
    s9.write(angle9-40+i9H0);
    i9H0++;
    if(i9H0==46){
      i9H0=0;
    }
    delay(actionspeed);
    s11.write(0+i11H0);
    i11H0++;
    if(i11H0==46){
      i11H0=0;
    }
    delay(actionspeed);
    if(status5>45){
      status5=0;
      break;
    }
    if(Serial.available()>0){
      break;
    }
    delay(actionspeed);
  }
  //battery();
 // sendbattery();
	
}




void QUANRUPED::read_mpu_6050_data()
{
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
//  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  delay(1);
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  temperature = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  gyro_x = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable
	
}
void QUANRUPED::setup_mpu_6050_registers()
{
	 //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
	
}
void QUANRUPED::self_balanced_setup()
{
	Wire.begin();                                                        //Start I2C as master
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-8g) and start the gyro
  for (int cal_int = 0; cal_int < 1000 ; cal_int ++){                  //Run this code 2000 times                            
    read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 1000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 1000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 1000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset
  loop_timer = micros();                                               //Reset the loop timer  
}
void QUANRUPED::self_balanced()
{
	read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050
  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value
  //Gyro angle calculations
  angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll
  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros();                                               //Reset the loop timer
	
}
void QUANRUPED::self_balanced_test()
{
	 //left-leaning 
    if(angle_pitch_output<-3 && angle_roll_output<5 && angle_roll_output>-5){
      s0.write(angle0+angle_pitch_output*2*2/12);
      s2.write(angle2+angle_pitch_output*2*2/12); 
      s4.write(angle4+angle_pitch_output*2*2/12);
      s6.write(angle6-angle_pitch_output*2*10/12);
      s8.write(angle8-angle_pitch_output*2*10/12); 
      s10.write(angle10-angle_pitch_output*2*10/12);
      
    }
    //right-leaning
    if(angle_pitch_output>3 && angle_roll_output<5 && angle_roll_output>-5){
      s0.write(angle0-angle_pitch_output*2*10/12);
      s2.write(angle2-angle_pitch_output*2*10/12); 
      s4.write(angle4-angle_pitch_output*2*10/12);
      s6.write(angle6+angle_pitch_output*2*2/12);
      s8.write(angle8+angle_pitch_output*2*2/12); 
      s10.write(angle10+angle_pitch_output*2*2/12);
     
    }
    //forward-leaning
  if(angle_roll_output>3 && angle_pitch_output<3 && angle_pitch_output>-3){
    s0.write(angle0-angle_roll_output*5*1/12);
    s6.write(angle6+angle_roll_output*5*1/12);
    s2.write(angle2-angle_roll_output*5*8/12); 
    s8.write(angle8+angle_roll_output*5*8/12);
    s4.write(angle4-angle_roll_output*5*9/12); 
    s10.write(angle10+angle_roll_output*5*9/12);
    
  }
  //backward-leaning
  if(angle_roll_output<-3 && angle_pitch_output<3 && angle_pitch_output>-3){
    s0.write(angle0+angle_roll_output*5*9/12);
    s6.write(angle6-angle_roll_output*5*9/12);
    s2.write(angle2+angle_roll_output*5*8/12); 
    s8.write(angle8-angle_roll_output*5*8/12);
    s4.write(angle4+angle_roll_output*5*1/12); 
    s10.write(angle10-angle_roll_output*5*1/12);
    

  }
	
}

void QUANRUPED::attack(){
	//  while(status4<=30){
//    delay(2);
//  status4++;
//  s12.write(90+i2H4);
//  i2H4++;
//  if(i2H4==31){
//    i2H4=0;
//  }
//  ultrasonic();
//  a+=a;
//  if(status4>30){
//  status4=0;
//  break;
//  }
//  if(Serial.available()>0){
//    break;
//  }
//  }
//
//  while(status4<=30){
//    delay(2);
//  status4++;
//  s12.write(120-i2H4);
//  i2H4++;
//  if(i2H4==31){
//    i2H4=0;
//  }
//  ultrasonic();
//  a+=a;
//  if(status4>30){
//  status4=0;
//  break;
//  }
//  if(Serial.available()>0){
//    break;
//  }
//  }
//
//  while(status4<=30){
//    delay(2);
//  status4++;
//  s12.write(90-i2H4);
//  i2H4++;
//  if(i2H4==31){
//    i2H4=0;
//  }
//  ultrasonic();
//  a+=a;
//  if(status4>30){
//  status4=0;
//  break;
//  }
//  if(Serial.available()>0){
//    break;
//  }
//  }
//
//  while(status4<=30){
//    delay(2);
//  status4++;
//  s12.write(60+i2H4);
//  i2H4++;
//  if(i2H4==31){
//    i2H4=0;
//  }
//  ultrasonic();
//  a+=a;
//  if(status4>30){
//  status4=0;
//  break;
//  }
//  if(Serial.available()>0){
//    break;
//  }
//  }
//  a=(a/120);
  ultrasonic();
  if(a<=15 && a>0){
  s0.write(angle0);
  s1.write(0); //45
  s3.write(0);//90
  s2.write(angle2-45); //10
  s4.write(angle4-45);
  s5.write(angle5-15); //120
  s6.write(angle6);//180
  s7.write(180); //90
  s8.write(angle8+45);
  s9.write(angle9+45); //95
  s11.write(angle11);//0 
  s10.write(angle10+45);   //180
  while(status4<=30){
    delay(10);
  status4++;
  s12.write(angle12+i2H4);
  i2H4++;
  if(i2H4==31){
    i2H4=0;
  }
  if(status4>30){
  status4=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
  }

  while(status4<=30){
    delay(10);
  status4++;
  s12.write(angle12+30-i2H4);
  i2H4++;
  if(i2H4==31){
    i2H4=0;
  }
  if(status4>30){
  status4=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
  }

  while(status4<=30){
    delay(10);
  status4++;
  s12.write(angle12-i2H4);
  i2H4++;
  if(i2H4==31){
    i2H4=0;
  }
  if(status4>30){
  status4=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
  }

  while(status4<=30){
    delay(10);
  status4++;
  s12.write(angle12-30+i2H4);
  i2H4++;
  if(i2H4==31){
    i2H4=0;
  }
  if(status4>30){
  status4=0;
  break;
  }
  if(Serial.available()>0){
    break;
  }
  }
//  s12.write(90);
//  delay(4000);
  }
  else if(a>15){
//  s0.write(45);
//  s1.write(90); //45
//  s3.write(45);//90
//  s2.write(45); //10
//  s4.write(45);
//  s5.write(75); //120
//  s6.write(135);//180
//  s7.write(90); //90
//  s8.write(135);
//  s9.write(95); //95
//  s11.write(90);//0 
//  s10.write(135);   //180
//  s12.write(90);
while(a>15){
  ultrasonic();
original_latest();
if(Serial.available()>0){
  break;
}
}
  }
//  battery();
//  sendbattery();
  
}



void QUANRUPED::steaty()
{
	self_balanced();s1.write(angle0); s3.write(angle3);s5.write(angle5);
	s7.write(angle7);s9.write(angle9); s11.write(angle11);self_balanced_test();
}


void QUANRUPED::sendultrasonic(){
  s12.write(angle);
    angle++;
    if(angle%3 == 0){
    ultrasonic();
    Serial.print("AT+CIPSEND=0,3\r\n");
    delay(20);
    Serial.println(a);
    delay(250);
   }
   if(angle==124){
    angle=20;
   }
}

void QUANRUPED::sendbattery(){
  while(1){
  battery_voltage =battery_voltage*0.92+((analogRead(A7) * 2.93) + 250)*0.08;
  showbattery = (battery_voltage-190)/2.93*5/1023*6;
  if(millis()-showtime>15000){
    Serial.println("AT+CIPSEND=0,6\r\n");
    delay(10);
    Serial.print("V:");
    Serial.println(showbattery);
//    delay(250);
    showtime = millis();
  }
  if(Serial.available()>0){
    break;
  }
 }
}
