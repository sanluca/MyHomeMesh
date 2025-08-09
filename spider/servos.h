#ifndef SERVOS_H
#define SERVOS_H

#include <Servo.h>

// Servo Definitions
extern Servo servo_left_front_a;
extern Servo servo_left_front_b;
extern Servo servo_left_middle_a;
extern Servo servo_left_middle_b;
extern Servo servo_left_rear_a;
extern Servo servo_left_rear_b;
extern Servo servo_right_front_a;
extern Servo servo_right_front_b;
extern Servo servo_right_middle_a;
extern Servo servo_right_middle_b;
extern Servo servo_right_rear_a;
extern Servo servo_right_rear_b;
extern Servo servo_head;

// Servo Angles
const int SERVO_NEUTRAL_ANGLE = 90;
const int SERVO_MIN_ANGLE = 45;
const int SERVO_MAX_ANGLE = 135;

// Movement Delays
const int STEP_DELAY_MS = 100;

#endif
