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


void tripod_gait_forward() {
    static int step = 0;
    static unsigned long last_update = 0;

    unsigned long interval = STEP_DELAY_MS;
    if (step == 1 || step == 4) { // Steps for shifting the body take longer
        interval = STEP_DELAY_MS * 2;
    }

    if (millis() - last_update > interval) {
        last_update = millis();

        switch(step) {
            case 0: // Lift Tripod 1
                servo_left_front_b.write(SERVO_MAX_ANGLE);
                servo_right_middle_b.write(SERVO_MAX_ANGLE);
                servo_left_rear_b.write(SERVO_MAX_ANGLE);
                break;
            case 1: // Shift body forward
                servo_left_front_a.write(SERVO_MIN_ANGLE);
                servo_right_middle_a.write(SERVO_MAX_ANGLE);
                servo_left_rear_a.write(SERVO_MIN_ANGLE);
                servo_right_front_a.write(SERVO_MIN_ANGLE);
                servo_left_middle_a.write(SERVO_MAX_ANGLE);
                servo_right_rear_a.write(SERVO_MIN_ANGLE);
                break;
            case 2: // Lower Tripod 1
                servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
                break;
            case 3: // Lift Tripod 2
                servo_right_front_b.write(SERVO_MAX_ANGLE);
                servo_left_middle_b.write(SERVO_MAX_ANGLE);
                servo_right_rear_b.write(SERVO_MAX_ANGLE);
                break;
            case 4: // Shift body forward
                servo_right_front_a.write(SERVO_MAX_ANGLE);
                servo_left_middle_a.write(SERVO_MIN_ANGLE);
                servo_right_rear_a.write(SERVO_MAX_ANGLE);
                servo_left_front_a.write(SERVO_MAX_ANGLE);
                servo_right_middle_a.write(SERVO_MIN_ANGLE);
                servo_left_rear_a.write(SERVO_MAX_ANGLE);
                break;
            case 5: // Lower Tripod 2
                servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
                break;
            case 6: // Return all 'a' servos to neutral
                servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 7;
    }
}

void turn_left() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_b.write(SERVO_MAX_ANGLE); break;
            case 1: servo_left_front_a.write(SERVO_MAX_ANGLE); break; // Swing backward
            case 2: servo_left_front_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 3: servo_left_rear_b.write(SERVO_MAX_ANGLE); break;
            case 4: servo_left_rear_a.write(SERVO_MAX_ANGLE); break; // Swing backward
            case 5: servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 6: servo_right_front_b.write(SERVO_MAX_ANGLE); break;
            case 7: servo_right_front_a.write(SERVO_MAX_ANGLE); break; // Swing forward
            case 8: servo_right_front_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 9: servo_right_rear_b.write(SERVO_MAX_ANGLE); break;
            case 10: servo_right_rear_a.write(SERVO_MAX_ANGLE); break; // Swing forward
            case 11: servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 12: // Reset to neutral
                servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 13;
    }
}

void turn_right() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        switch(step) {
            case 0: servo_left_front_b.write(SERVO_MAX_ANGLE); break;
            case 1: servo_left_front_a.write(SERVO_MIN_ANGLE); break; // Swing forward
            case 2: servo_left_front_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 3: servo_left_rear_b.write(SERVO_MAX_ANGLE); break;
            case 4: servo_left_rear_a.write(SERVO_MIN_ANGLE); break; // Swing forward
            case 5: servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 6: servo_right_front_b.write(SERVO_MAX_ANGLE); break;
            case 7: servo_right_front_a.write(SERVO_MIN_ANGLE); break; // Swing backward
            case 8: servo_right_front_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 9: servo_right_rear_b.write(SERVO_MAX_ANGLE); break;
            case 10: servo_right_rear_a.write(SERVO_MIN_ANGLE); break; // Swing backward
            case 11: servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 12: // Reset to neutral
                servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 13;
    }
}

void head_straight() { servo_head.write(SERVO_NEUTRAL_ANGLE); }
void head_left() { servo_head.write(SERVO_MAX_ANGLE); }
void head_right() { servo_head.write(SERVO_MIN_ANGLE); }

void body_up() {
    int angle = SERVO_NEUTRAL_ANGLE - 30;
    servo_left_front_b.write(angle);
    servo_left_middle_b.write(angle);
    servo_left_rear_b.write(angle);
    servo_right_front_b.write(angle);
    servo_right_middle_b.write(angle);
    servo_right_rear_b.write(angle);
}

void body_down() {
    servo_left_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_front_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE);
    servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE);
}

void scared_animation() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();
        switch(step) {
            case 0: // Retract legs
                servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
                break;
            case 1: // Lower body
                body_down();
                break;
            // Case 2-5: Wait
            case 6: // Reset step counter for next time
                step = -1; // so it becomes 0 after increment
                break;
        }
        if (step < 6) step++;
    }
}

void observe_animation() {
    static int step = 0;
    static unsigned long last_update = 0;
    const unsigned int intervals[] = { 300, 500, 500, 200 };

    if (millis() - last_update > intervals[step]) {
        last_update = millis();
        switch(step) {
            case 0: head_straight(); break;
            case 1: head_left(); break;
            case 2: head_right(); break;
            case 3: head_straight(); break;
        }
        step = (step + 1) % 4;
    }
}

// The old blocking functions are removed as they are replaced by non-blocking versions.
void move_forward() {}

void move_backward() {
    static int step = 0;
    static unsigned long last_update = 0;

    if (millis() - last_update > STEP_DELAY_MS) {
        last_update = millis();

        // This is a simple ripple gait for moving backward.
        // It moves one leg at a time.
        switch(step) {
            // LF leg
            case 0: servo_left_front_b.write(SERVO_MAX_ANGLE); break;
            case 1: servo_left_front_a.write(SERVO_MAX_ANGLE); break; // Backward
            case 2: servo_left_front_b.write(SERVO_NEUTRAL_ANGLE); break;
            // RF leg
            case 3: servo_right_front_b.write(SERVO_MAX_ANGLE); break;
            case 4: servo_right_front_a.write(SERVO_MIN_ANGLE); break; // Backward
            case 5: servo_right_front_b.write(SERVO_NEUTRAL_ANGLE); break;
            // LM leg
            case 6: servo_left_middle_b.write(SERVO_MAX_ANGLE); break;
            case 7: servo_left_middle_a.write(SERVO_MAX_ANGLE); break; // Backward
            case 8: servo_left_middle_b.write(SERVO_NEUTRAL_ANGLE); break;
            // RM leg
            case 9: servo_right_middle_b.write(SERVO_MAX_ANGLE); break;
            case 10: servo_right_middle_a.write(SERVO_MIN_ANGLE); break; // Backward
            case 11: servo_right_middle_b.write(SERVO_NEUTRAL_ANGLE); break;
            // LR leg
            case 12: servo_left_rear_b.write(SERVO_MAX_ANGLE); break;
            case 13: servo_left_rear_a.write(SERVO_MAX_ANGLE); break; // Backward
            case 14: servo_left_rear_b.write(SERVO_NEUTRAL_ANGLE); break;
            // RR leg
            case 15: servo_right_rear_b.write(SERVO_MAX_ANGLE); break;
            case 16: servo_right_rear_a.write(SERVO_MIN_ANGLE); break; // Backward
            case 17: servo_right_rear_b.write(SERVO_NEUTRAL_ANGLE); break;

            case 18: // Reset all 'a' servos to neutral
                servo_left_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_front_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_middle_a.write(SERVO_NEUTRAL_ANGLE);
                servo_left_rear_a.write(SERVO_NEUTRAL_ANGLE);
                servo_right_rear_a.write(SERVO_NEUTRAL_ANGLE);
                break;
        }
        step = (step + 1) % 19;
    }
}

void head_down() {}