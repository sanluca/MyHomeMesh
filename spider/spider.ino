// spider adeept ADA033-V5.0
// Final version with non-blocking state machine and behaviors
//arduino uno
#include "movimento_robot.h"
#include "servos.h"
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_NeoPixel.h>

// --- Hardware Definitions ---
#define LED_COUNT 6
#define LED_PIN A1
const int ULTRASONIC_ECHO_PIN = A3;
const int ULTRASONIC_TRIG_PIN = A2;

// --- Global Objects ---
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
MPU6050 mpu(Wire);

// --- State Machine ---
enum RobotState { IDLE, EXPLORING, ASSESSING_OBSTACLE, EVADING, OBSERVING, ALERT, SUSPENDED };
RobotState currentState = IDLE;
enum EvasionPath { PATH_LEFT, PATH_RIGHT, PATH_BLOCKED };
EvasionPath chosenPath = PATH_BLOCKED;

// --- Thresholds & Constants ---
const int OBSTACLE_DISTANCE_CM = 15;
const float BUMP_THRESHOLD = 1.5;
const float TILT_THRESHOLD_DEGREES = 20.0;
const unsigned long OBSERVE_INTERVAL_MS = 15000;
const unsigned long ULTRASONIC_INTERVAL_MS = 100;
const unsigned long EVASION_DURATION_MS = 2000; // Turn for 2 seconds
const unsigned long ALERT_DURATION_MS = 2000;
const unsigned long OBSERVE_DURATION_MS = 3000;
const unsigned long ASSESSMENT_STEP_INTERVAL_MS = 400;

// --- Timers & Global Variables ---
unsigned long last_state_change_time = 0;
unsigned long last_observe_time = 0;
unsigned long last_ultrasonic_read = 0;
int distance_cm = 0;

// --- Utility Functions ---
void change_state(RobotState newState) {
    Serial.print("Changing state from ");
    Serial.print(currentState);
    Serial.print(" to ");
    Serial.println(newState);
    currentState = newState;
    last_state_change_time = millis();
}

void set_all_leds(int r, int g, int b) {
    for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}

long microseconds_to_centimeters(long microseconds) {
    return microseconds / 29 / 2;
}

void read_distance_sensor() {
    if (millis() - last_ultrasonic_read > ULTRASONIC_INTERVAL_MS) {
        last_ultrasonic_read = millis();
        long duration;
        digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
        duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 50000); // 50ms timeout
        if (duration > 0) {
            distance_cm = microseconds_to_centimeters(duration);
        } else {
            distance_cm = 999;
        }
    }
}

bool check_for_bump() {
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float totalAcc = sqrt(accX * accX + accY * accY);
    return (totalAcc > BUMP_THRESHOLD);
}

bool check_for_tilt() {
    float angleX = mpu.getAngleX();
    float angleY = mpu.getAngleY();
    return (abs(angleX) > TILT_THRESHOLD_DEGREES || abs(angleY) > TILT_THRESHOLD_DEGREES);
}

// --- State Handlers ---

void handle_idle() {
    rest();
    if (millis() - last_state_change_time > 1000) {
        change_state(EXPLORING);
    }
}

void handle_exploring() {
    set_all_leds(0, 100, 0); // Dim Green
    move_forward(); // Use the original, non-blocking ripple gait

    if (check_for_tilt()) {
        rest();
        change_state(SUSPENDED);
    } else if (distance_cm < OBSTACLE_DISTANCE_CM) {
        rest();
        change_state(ASSESSING_OBSTACLE);
    } else if (check_for_bump()) {
        rest();
        change_state(ALERT);
    } else if (millis() - last_observe_time > OBSERVE_INTERVAL_MS) {
        rest();
        change_state(OBSERVING);
    }
}

void handle_assessing_obstacle() {
    static int assessment_step = 0;
    static unsigned long last_assessment_update = 0;
    static int left_dist = 0, right_dist = 0;

    set_all_leds(150, 150, 0); // Dim Yellow

    if (millis() - last_assessment_update > ASSESSMENT_STEP_INTERVAL_MS) {
        last_assessment_update = millis();

        switch (assessment_step) {
            case 0:
                rest();
                head_left();
                assessment_step++;
                break;
            case 1:
                left_dist = distance_cm;
                Serial.print("Left dist: "); Serial.println(left_dist);
                head_right();
                assessment_step++;
                break;
            case 2:
                right_dist = distance_cm;
                Serial.print("Right dist: "); Serial.println(right_dist);
                head_straight();
                assessment_step++;
                break;
            case 3:
                if (left_dist > right_dist && left_dist > OBSTACLE_DISTANCE_CM) {
                    chosenPath = PATH_LEFT;
                } else if (right_dist > OBSTACLE_DISTANCE_CM) {
                    chosenPath = PATH_RIGHT;
                } else {
                    chosenPath = PATH_BLOCKED;
                }
                assessment_step = 0; // Reset for next time
                change_state(EVADING);
                break;
        }
    }
}

void handle_evading() {
    set_all_leds(0, 0, 150); // Dim Blue
    if (chosenPath == PATH_LEFT) {
        turn_left();
    } else if (chosenPath == PATH_RIGHT) {
        turn_right();
    } else { // Blocked
        move_backward();
    }

    if (millis() - last_state_change_time > EVASION_DURATION_MS) {
        rest();
        change_state(EXPLORING);
    }
}

void handle_observing() {
    set_all_leds(100, 0, 150); // Purple
    head_left();
    if (millis() - last_state_change_time > OBSERVE_DURATION_MS/2) {
      head_right();
    }

    if (millis() - last_state_change_time > OBSERVE_DURATION_MS) {
        rest();
        last_observe_time = millis();
        change_state(EXPLORING);
    }
}

void handle_alert() {
    // Flash LEDs
    if ((millis() / 200) % 2 == 0) set_all_leds(255,0,0);
    else set_all_leds(0,0,0);

    if (millis() - last_state_change_time > ALERT_DURATION_MS) {
        rest();
        change_state(ASSESSING_OBSTACLE);
    }
}

void handle_suspended() {
    set_all_leds(255, 255, 255); // White
    rest();
    // Check if the robot is placed back on a flat surface
    if (!check_for_tilt()) {
        // Wait a moment to ensure it's stable
        if (millis() - last_state_change_time > 1000) {
            change_state(IDLE);
        }
    } else {
        // If it's still tilted, reset the timer
        last_state_change_time = millis();
    }
}

// --- Main Setup and Loop ---

void setup() {
    Serial.begin(9600);
    attach_servos();
    Wire.begin();
    mpu.begin();
    strip.begin();

    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);

    strip.setBrightness(50);
    set_all_leds(0, 0, 0);

    Serial.println("Calibrating MPU6050...");
    mpu.calcOffsets(true, true);
    Serial.println("Calibration Done!");

    rest();
    Serial.println("Spider robot initialized. Starting in IDLE state.");
    change_state(IDLE);
}

void loop() {
    mpu.update();
    read_distance_sensor();

    switch (currentState) {
        case IDLE: handle_idle(); break;
        case EXPLORING: handle_exploring(); break;
        case ASSESSING_OBSTACLE: handle_assessing_obstacle(); break;
        case EVADING: handle_evading(); break;
        case OBSERVING: handle_observing(); break;
        case ALERT: handle_alert(); break;
        case SUSPENDED: handle_suspended(); break;
    }
}