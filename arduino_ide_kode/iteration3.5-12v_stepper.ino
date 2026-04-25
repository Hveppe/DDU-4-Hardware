#include <Arduino.h>
#include <AccelStepper.h>

// --------------------
// PIN SETUP
// --------------------
const int dirPin = 2;
const int stepPin = 3;
const int enablePin = 9;

// Driver control pins
const int sleepPin = 4;
const int resetPin = 5;
const int MS0 = 6;
const int MS1 = 7;
const int MS2 = 8;

// --------------------
// STEPPER SETUP
// --------------------
#define motorInterfaceType 1

AccelStepper stepper(motorInterfaceType, stepPin, dirPin);

// --------------------
// CONTROL VARIABLES
// --------------------
int targetSpeed = 0;     // current commanded speed
int maxSpeed = 2000;     // adjust depending on motor
int accel = 800;

// --------------------
// SETUP
// --------------------
void setup() {
  Serial.begin(115200);

  // Stepper config
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(accel);

  // Pin setup
  pinMode(enablePin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);

  // Microstepping (1/16 = smooth)
  digitalWrite(MS0, HIGH);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);

  // Enable driver
  digitalWrite(enablePin, LOW);
  digitalWrite(sleepPin, HIGH);
  digitalWrite(resetPin, HIGH);

  Serial.println("READY");
}

// --------------------
// LOOP
// --------------------
void loop() {

  // --- SERIAL ---
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {

      case 'R': // højre
        targetSpeed = 1000;
        Serial.write("A", 1);
        break;

      case 'L': // venstre
        targetSpeed = -1000;
        Serial.write("A", 1);
        break;

      case 'C': // stop
        targetSpeed = 0;
        Serial.write("A", 1);
        break;

      case 'S': // sweep
        targetSpeed = 600;
        Serial.write("A", 1);
        break;

      default:
        Serial.write("E", 1);
        break;
    }
  }

  // --- APPLY SPEED ---
  stepper.setSpeed(targetSpeed);
  stepper.runSpeed();
}