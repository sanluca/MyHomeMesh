/***********************************************************
File name:MPU6050.ino
Description: Get the X, Y and Z axis data of MPU6050.
Website: www.adeept.com
E-mail: support@adeept.com
Author: Tom
Date: 2019/05/22
***********************************************************/
#include<Wire.h>
unsigned long now, lastTime = 0;
float dt;                                   //derivative time
unsigned short times = 200;             //Sampling frequency
int16_t ax, ay, az, gx, gy, gz;             //Accelerometer gyroscope raw data
float aax=0, aay=0,aaz=0, agx=0, agy=0, agz=0;    //Angle variable
long axo = 0, ayo = 0, azo = 0;             //Accelerometer offset
long gxo = 0, gyo = 0, gzo = 0;             //Gyroscope offset
 
float pi = 3.1415926;
float AcceRatio = 16384.0;                  //Accelerometer scaling factor
float GyroRatio = 131.0;                    //Gyroscope scale factor
 
uint8_t n_sample = 8;                       //The number of samples sampled by accelerometer filtering algorithm
float aaxs[8] = {0}, aays[8] = {0}, aazs[8] = {0};         //Sample the queue on the x and y axes
long aax_sum, aay_sum,aaz_sum;                      //X,y axis sampling queue x,y axis sampling and
 
float a_x[10]={0}, a_y[10]={0},a_z[10]={0} ,g_x[10]={0} ,g_y[10]={0},g_z[10]={0}; //Accelerometer covariance calculation queue
float Px=1, Rx, Kx, Sx, Vx, Qx;             //The kalman variable on the X-axis
float Py=1, Ry, Ky, Sy, Vy, Qy;             //The kalman variable on the Y-axis
float Pz=1, Rz, Kz, Sz, Vz, Qz;             //The kalman variable on the Z-axis

const int MPU_addr=0x68; // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 

  Serial.begin(115200);

   for(int i=0;i<times;i++)
      {
          getMotion6(); // read the original value of six axes
          axo += AcX; ayo += AcY; azo += AcZ;      //sampling interval
          gxo += GyX; gyo += GyY; gzo += GyZ;
      }
      axo /= times; ayo /= times; azo /= times; //Calculate the accelerometer offset
      gxo /= times; gyo /= times; gzo /= times; //Calculate the accelerometer offset


}
int x = 60,y = 80;
int count_x = 0;
float p = 0.2;
float r_x = 60.0,r_y=80.0;
int before_x,before_y;
void loop()
{
  count6Axle();
}
void getMotion6()
{
   Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
}

void count6Axle()
{
  unsigned long now = millis();             // current time (ms)
    dt = (now - lastTime) / 1000.0;           // differential time (s)
    lastTime = now;                           // last sampling time (ms)
    getMotion6();
     float accx = AcX / AcceRatio;              //x axis acceleration
    float accy = AcY / AcceRatio;              //y axis acceleration
    float accz = AcZ / AcceRatio;              ///Z axis acceleration
 
    aax = atan(accy / accz) * (-180) / pi;    // the Angle between the Y-axis and the z-axis
    aay = atan(accx / accz) * 180 / pi;       // the Angle between the X-axis and the z-axis
    aaz = atan(accz / accy) * 180 / pi;       // the Angle between the z axis and the y axis
 
    aax_sum = 0;                              // the sliding weighted filtering algorithm for the original accelerometer data
    aay_sum = 0;
    aaz_sum = 0;
     for(int i=1;i<n_sample;i++)
    {
        aaxs[i-1] = aaxs[i];
        aax_sum += aaxs[i] * i;
        aays[i-1] = aays[i];
        aay_sum += aays[i] * i;
        aazs[i-1] = aazs[i];
        aaz_sum += aazs[i] * i;
    
    }
 
    aaxs[n_sample-1] = aax;
    aax_sum += aax * n_sample;
    aax = (aax_sum / (11*n_sample/2.0)) * 9 / 7.0; // Angle am to 0-90°
    aays[n_sample-1] = aay;                        // here the appropriate coefficient is obtained by the experimental method
    aay_sum += aay * n_sample;                     // the coefficient in this example is 9/7
    aay = (aay_sum / (11*n_sample/2.0)) * 9 / 7.0;
    aazs[n_sample-1] = aaz; 
    aaz_sum += aaz * n_sample;
    aaz = (aaz_sum / (11*n_sample/2.0)) * 9 / 7.0;
 
    float gyrox = - (GyX-gxo) / GyroRatio * dt; // angular velocity in the X-axis
    float gyroy = - (GyY-gyo) / GyroRatio * dt; // angular velocity in the Y-axis
    float gyroz = - (GyZ-gzo) / GyroRatio * dt; // angular velocity in the Z-axis
    agx += gyrox;                             //Angular velocity integral
    agy += gyroy;                             
    agz += gyroz;

      /* kalman start */
    Sx = 0; Rx = 0;
    Sy = 0; Ry = 0;
    Sz = 0; Rz = 0;
    
    for(int i=1;i<10;i++)
    {                 // average calculation of measured values
        a_x[i-1] = a_x[i];                      // is the average acceleration
        Sx += a_x[i];
        a_y[i-1] = a_y[i];
        Sy += a_y[i];
        a_z[i-1] = a_z[i];
        Sz += a_z[i];
    
    }
    
    a_x[9] = aax;
    Sx += aax;
    Sx /= 10;                                 // average acceleration of the X-axis
    a_y[9] = aay;
    Sy += aay;
    Sy /= 10;                                 // average acceleration of the Y-axis
    a_z[9] = aaz;
    Sz += aaz;
    Sz /= 10;
 
    for(int i=0;i<10;i++)
    {
        Rx += sq(a_x[i] - Sx);
        Ry += sq(a_y[i] - Sy);
        Rz += sq(a_z[i] - Sz);
    
    }
    
    Rx = Rx / 9;                              // to get the variance
    Ry = Ry / 9;                        
    Rz = Rz / 9;
  
    Px = Px + 0.0025;                         
    Kx = Px / (Px + Rx);                     // calculate the kalman gain
    agx = agx + Kx * (aax - agx);             // the gyroscope Angle is superimposed with the accelerometer velocity
    Px = (1 - Kx) * Px;                       // update the p value
 
    Py = Py + 0.0025;
    Ky = Py / (Py + Ry);
    agy = agy + Ky * (aay - agy); 
    Py = (1 - Ky) * Py;
  
    Pz = Pz + 0.0025;
    Kz = Pz / (Pz + Rz);
    agz = agz + Kz * (aaz - agz); 
    Pz = (1 - Kz) * Pz;
      /* kalman end */
 
    Serial.print(agx);Serial.print(",");
    Serial.print(agy);Serial.print(",");
    Serial.print(agz);Serial.println();

}
