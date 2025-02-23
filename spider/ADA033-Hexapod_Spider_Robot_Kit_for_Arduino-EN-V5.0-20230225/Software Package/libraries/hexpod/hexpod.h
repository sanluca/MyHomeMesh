#ifndef _HEXPOD_h__
#define _HEXPOD_h__
#include "Arduino.h"

 class QUANRUPED
 {
	public:
		QUANRUPED();
		~QUANRUPED();
		void servo_attach();
		void moveforward();
		void movebackward();
		void turnright();
		void turnleft();
		void self_balanced_test();
		void battery();
		void advoid();
		
		void self_balanced_setup();
		void self_balanced();
		void read_mpu_6050_data();
		void setup_mpu_6050_registers();
		void ultrasonic();
		void strip_begin(int r_data,int g_data,int b_data);
		void servo_init();
		void attack();
		void sendbattery();
		
		void steaty();
		void sendultrasonic();
		
		
 };
 #endif