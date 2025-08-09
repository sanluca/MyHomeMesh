#include "movimento_robot.h"
#include "servos.h"
#include <Arduino.h>

// This file is refactored to be non-blocking.
// All movement functions must be called repeatedly in the main loop.
// They use static variables to keep track of their state and millis() for timing.
// Calling rest() will reset the robot to a neutral position and stop any ongoing movement.

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
    // This function is blocking as it's a reset to a known state.
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
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_a.write(SERVO_MIN_ANGLE); break;
            case 1: servo_left_front_b.write(SERVO_MAX_ANGLE); break;
            case 2: servo_left_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 3: servo_left_rear_a.write(SERVO_MIN_ANGLE); break;
            case 4: servo_left_rear_b.write(SERVO_MAX_ANGLE); break;
            case 5: servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 6: servo_right_middle_a.write(SERVO_MAX_ANGLE); break;
            case 7: servo_right_middle_b.write(SERVO_MIN_ANGLE); break;
            case 8: servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 9:
                servo_right_front_a.write(SERVO_MAX_ANGLE);
                servo_left_middle_a.write(SERVO_MIN_ANGLE);
                servo_right_rear_a.write(SERVO_MAX_ANGLE);
                break;
            case 10:
                servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
                break;
            case 11: servo_right_front_a.write(SERVO_MAX_ANGLE); break;
            case 12: servo_right_front_b.write(SERVO_MIN_ANGLE); break;
            case 13: servo_right_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 14: servo_left_middle_a.write(SERVO_MIN_ANGLE); break;
            case 15: servo_left_middle_b.write(SERVO_MAX_ANGLE); break;
            case 16: servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 17: servo_right_rear_a.write(SERVO_MAX_ANGLE); break;
            case 18: servo_right_rear_b.write(SERVO_MIN_ANGLE); break;
            case 19: servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 20:
                servo_left_front_a.write(SERVO_MIN_ANGLE);
                servo_left_rear_a.write(SERVO_MIN_ANGLE);
                servo_right_middle_a.write(SERVO_MAX_ANGLE);
                break;
            case 21:
                servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 22;
    }
}

void move_backward() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_a.write(SERVO_MIN_ANGLE); break;
            case 1: servo_left_front_b.write(SERVO_MIN_ANGLE); break;
            case 2: servo_left_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 3: servo_left_rear_a.write(SERVO_MIN_ANGLE); break;
            case 4: servo_left_rear_b.write(SERVO_MIN_ANGLE); break;
            case 5: servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 6: servo_right_middle_a.write(SERVO_MAX_ANGLE); break;
            case 7: servo_right_middle_b.write(SERVO_MAX_ANGLE); break;
            case 8: servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 9:
                servo_right_front_a.write(SERVO_MAX_ANGLE);
                servo_left_middle_a.write(SERVO_MIN_ANGLE);
                servo_right_rear_a.write(SERVO_MAX_ANGLE);
                break;
            case 10:
                servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
                break;
            case 11: servo_right_front_a.write(SERVO_MAX_ANGLE); break;
            case 12: servo_right_front_b.write(SERVO_MAX_ANGLE); break;
            case 13: servo_right_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 14: servo_left_middle_a.write(SERVO_MIN_ANGLE); break;
            case 15: servo_left_middle_b.write(SERVO_MIN_ANGLE); break;
            case 16: servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 17: servo_right_rear_a.write(SERVO_MAX_ANGLE); break;
            case 18: servo_right_rear_b.write(SERVO_MAX_ANGLE); break;
            case 19: servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 20:
                servo_left_front_a.write(SERVO_MIN_ANGLE);
                servo_left_rear_a.write(SERVO_MIN_ANGLE);
                servo_right_middle_a.write(SERVO_MAX_ANGLE);
                break;
            case 21:
                servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 22;
    }
}

void head_down() {
    // This function is simple, can remain blocking or be converted if needed.
    // For now, it's not used in the main logic.
}

void turn_left() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_a.write(SERVO_MIN_ANGLE); break;
            case 1: servo_left_front_b.write(SERVO_MIN_ANGLE); break;
            case 2: servo_left_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 3: servo_left_middle_a.write(SERVO_MIN_ANGLE); break;
            case 4: servo_left_middle_b.write(SERVO_MIN_ANGLE); break;
            case 5: servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 6: servo_left_rear_a.write(SERVO_MIN_ANGLE); break;
            case 7: servo_left_rear_b.write(SERVO_MIN_ANGLE); break;
            case 8: servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 9: servo_right_front_a.write(SERVO_MAX_ANGLE); break;
            case 10: servo_right_front_b.write(SERVO_MIN_ANGLE); break;
            case 11: servo_right_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 12: servo_right_middle_a.write(SERVO_MAX_ANGLE); break;
            case 13: servo_right_middle_b.write(SERVO_MIN_ANGLE); break;
            case 14: servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 15: servo_right_rear_a.write(SERVO_MAX_ANGLE); break;
            case 16: servo_right_rear_b.write(SERVO_MIN_ANGLE); break;
            case 17: servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 18: rest(); break;
        }
        step = (step + 1) % 19;
    }
}

void turn_right() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_a.write(SERVO_MIN_ANGLE); break;
            case 1: servo_left_front_b.write(SERVO_MAX_ANGLE); break;
            case 2: servo_left_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 3: servo_left_middle_a.write(SERVO_MIN_ANGLE); break;
            case 4: servo_left_middle_b.write(SERVO_MAX_ANGLE); break;
            case 5: servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 6: servo_left_rear_a.write(SERVO_MIN_ANGLE); break;
            case 7: servo_left_rear_b.write(SERVO_MAX_ANGLE); break;
            case 8: servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 9: servo_right_front_a.write(SERVO_MAX_ANGLE); break;
            case 10: servo_right_front_b.write(SERVO_MAX_ANGLE); break;
            case 11: servo_right_front_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 12: servo_right_middle_a.write(SERVO_MAX_ANGLE); break;
            case 13: servo_right_middle_b.write(SERVO_MAX_ANGLE); break;
            case 14: servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 15: servo_right_rear_a.write(SERVO_MAX_ANGLE); break;
            case 16: servo_right_rear_b.write(SERVO_MAX_ANGLE); break;
            case 17: servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE); break;
            case 18: rest(); break;
        }
        step = (step + 1) % 19;
    }
}

void head_right() {
    servo_head.write(SERVO_MIN_ANGLE);
}

void head_left() {
    servo_head.write(SERVO_MAX_ANGLE);
}

void head_straight() {
    servo_head.write(SERVO_NEUTRAL_ANGLE);
}