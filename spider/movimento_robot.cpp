#include "movimento_robot.h"
#include "servos.h"
#include <Arduino.h>

Servo servo_left_front_a;
Servo servo_left_front_b;
Servo servo_left_middle_a;
Servo servo_left_middle_b;
Servo servo_left_rear_a;
Servo servo_left_rear_b;
Servo servo_right_front_a;
Servo servo_right_front_b;
Servo servo_right_middle_a;
Servo servo_right_middle_b;
Servo servo_right_rear_a;
Servo servo_right_rear_b;
Servo servo_head;

void attach_servos() {
    servo_left_front_a.attach(2);
    servo_left_front_b.attach(3);
    servo_left_middle_a.attach(4);
    servo_left_middle_b.attach(5);
    servo_left_rear_a.attach(6);
    servo_left_rear_b.attach(7);
    servo_right_front_a.attach(8);
    servo_right_front_b.attach(9);
    servo_right_middle_a.attach(10);
    servo_right_middle_b.attach(11);
    servo_right_rear_a.attach(12);
    servo_right_rear_b.attach(13);
    servo_head.attach(A0);
}

void rest() {
    servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
    servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
    servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_head.write(SERVO_NEUTRAL_ANGLE);
}

void move_forward() {
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
}

void move_backward() {
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
}

void head_down() {
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_front_a.write(SERVO_MAX_ANGLE);
    servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
}

void turn_left() {
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_b.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
    rest();
    delay(STEP_DELAY_MS);
}

void turn_right() {
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_middle_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_left_rear_a.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_front_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_middle_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);

    servo_right_rear_a.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_b.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
    servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);

    delay(STEP_DELAY_MS);
    rest();
    delay(STEP_DELAY_MS);
}

void head_right() {
    delay(STEP_DELAY_MS);
    servo_head.write(SERVO_MIN_ANGLE);
    delay(STEP_DELAY_MS);
}

void head_left() {
    delay(STEP_DELAY_MS);
    servo_head.write(SERVO_MAX_ANGLE);
    delay(STEP_DELAY_MS);
}

void head_straight() {
    delay(STEP_DELAY_MS);
    servo_head.write(SERVO_NEUTRAL_ANGLE);
    delay(STEP_DELAY_MS);
}