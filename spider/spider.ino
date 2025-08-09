// Adeept Hexapod Spider Robot ADA033-V5.0
//board arduino uno
#define NUM_SAMPLES 5 // Number of readings for moving average

#include "movimento_robot.h"
#include "servos.h"
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 6
#define LED_PIN A1
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

MPU6050 mpu(Wire);

const int ULTRASONIC_ECHO_PIN = A3;
const int ULTRASONIC_TRIG_PIN = A2;
int distance_cm = 0;

bool is_following = false;

// --- Thresholds ---
const int START_FOLLOW_DISTANCE_CM = 40;
const int STOP_FOLLOW_DISTANCE_CM = 100;
const float MOVEMENT_THRESHOLD = 0.005;
const float TILT_THRESHOLD = 10.0;
const float GYRO_THRESHOLD = 0.5;

void set_led(int led_number, int r, int g, int b) {
    if (led_number >= 0 && led_number < LED_COUNT) {
        strip.setPixelColor(led_number, strip.Color(r, g, b));
        strip.show();
    }
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

int get_distance_cm() {
    long duration;
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
    return microseconds_to_centimeters(duration);
}

void print_mpu_data() {
    Serial.print(F("TEMPERATURE: ")); Serial.println(mpu.getTemp());
    Serial.print(F("ACCELERO  X: ")); Serial.print(mpu.getAccX());
    Serial.print("\tY: "); Serial.print(mpu.getAccY());
    Serial.print("\tZ: "); Serial.println(mpu.getAccZ());
    Serial.print(F("GYRO      X: ")); Serial.print(mpu.getGyroX());
    Serial.print("\tY: "); Serial.print(mpu.getGyroY());
    Serial.print("\tZ: "); Serial.println(mpu.getGyroZ());
    Serial.print(F("ACC ANGLE X: ")); Serial.print(mpu.getAccAngleX());
    Serial.print("\tY: "); Serial.println(mpu.getAccAngleY());
    Serial.print(F("ANGLE     X: ")); Serial.print(mpu.getAngleX());
    Serial.print("\tY: "); Serial.print(mpu.getAngleY());
    Serial.print("\tZ: "); Serial.println(mpu.getAngleZ());
    Serial.println(F("=====================================================\n"));
}

bool is_robot_stuck() {
    static float accX_avg = 0;
    static float accY_avg = 0;
    static int count = 0;

    // print_mpu_data(); // Uncomment for debugging

    float accX = abs(mpu.getAccX());
    float accY = abs(mpu.getAccY());
    float gyroZ = abs(mpu.getGyroZ());
    float tiltX = abs(mpu.getAccAngleX());
    float tiltY = abs(mpu.getAccAngleY());

    accX_avg = ((accX_avg * count) + accX) / (count + 1);
    accY_avg = ((accY_avg * count) + accY) / (count + 1);
    count = min(count + 1, NUM_SAMPLES);

    bool stopped = (accX_avg < MOVEMENT_THRESHOLD && accY_avg < MOVEMENT_THRESHOLD);
    bool tilted = (tiltX > TILT_THRESHOLD || tiltY > TILT_THRESHOLD);

    if (gyroZ > GYRO_THRESHOLD) {
        return false; // Robot is turning
    }

    if (stopped || tilted) {
        Serial.println("Robot is STUCK!");
        return true;
    }

    return false;
}

void handle_stuck() {
    Serial.println("Robot is stuck! Trying to get out.");
    set_all_leds(255, 0, 0); // Red
    move_backward();
    delay(1000);
    move_backward();
    delay(1000);
    move_backward();
    delay(1000);
    head_straight();
    delay(1000);
    for (int i = 0; i < 7; i++) {
        turn_right();
    }
    delay(1000);
}

void handle_obstacle() {
    set_all_leds(0, 0, 255); // Blue
    head_right();
    delay(1000);
    distance_cm = get_distance_cm();
    if (distance_cm > START_FOLLOW_DISTANCE_CM) {
        for (int i = 0; i < 5; i++) {
            turn_right();
        }
        delay(1000);
        head_straight();
        delay(1000);
        move_forward();
        delay(500);
    } else {
        head_left();
        delay(1000);
        distance_cm = get_distance_cm();
        if (distance_cm > START_FOLLOW_DISTANCE_CM) {
            for (int i = 0; i < 5; i++) {
                turn_left();
            }
            delay(1000);
            head_straight();
            delay(1000);
            move_forward();
            delay(500);
        } else {
            handle_stuck(); // Trapped, try to get out
        }
    }
}

void follow_object() {
    if (distance_cm > START_FOLLOW_DISTANCE_CM) {
        move_forward();
        delay(500);
    } else {
        handle_obstacle();
    }
}

void setup() {
    Serial.begin(9600);

    attach_servos();
    rest();
    delay(1000);

    Wire.begin();
    mpu.begin();
    mpu.calcOffsets(true, true);

    strip.begin();
    strip.setBrightness(50);

    pinMode(ULTRASONIC_ECHO_PIN, INPUT);
    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);

    // LED startup sequence
    set_led(0, 255, 0, 0);
    delay(100);
    set_led(1, 0, 255, 0);
    delay(100);
    set_led(2, 0, 0, 255);
    delay(100);
    set_led(3, 255, 255, 0);
    delay(100);
    set_led(4, 0, 255, 255);
    delay(100);
    set_led(5, 255, 0, 255);
    delay(100);
    set_all_leds(0, 0, 0);
}

void loop() {
    distance_cm = get_distance_cm();
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    set_all_leds(0, 255, 0); // Green
    mpu.update();

    if (is_robot_stuck()) {
        handle_stuck();
    } else {
        if (distance_cm < START_FOLLOW_DISTANCE_CM && !is_following) {
            is_following = true;
            Serial.println("Following started!");
        } else if (distance_cm > STOP_FOLLOW_DISTANCE_CM && is_following) {
            is_following = false;
            Serial.println("Following stopped!");
            rest();
        }

        if (is_following) {
            follow_object();
        } else {
            rest();
        }
    }
}