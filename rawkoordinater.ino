#include <AccelStepper.h>

// -------------------- X --------------------
const int stepPinX = 3;
const int dirPinX  = 2;
const int enableX  = 9;
const int sleepX   = 4;
const int resetX   = 5;
const int ms1X     = 8;
const int ms2X     = 7;
const int ms3X     = 6;

// -------------------- Y --------------------
const int stepPinY = A0;
const int dirPinY  = A1;
const int enableY  = 13;
const int sleepY   = A2;
const int resetY   = A3;
const int ms1Y     = 10;
const int ms2Y     = 11;
const int ms3Y     = 12;

// -------------------- STEPPERS --------------------
AccelStepper stepperX(AccelStepper::DRIVER, stepPinX, dirPinX);
AccelStepper stepperY(AccelStepper::DRIVER, stepPinY, dirPinY);

// -------------------- SETTINGS --------------------
String input = "";

// 🔥 FIX FOR SPEJLVENDING (ændr hvis nødvendigt)
bool invertX = false;
bool invertY = false;

void setup() {
  Serial.begin(115200);

  pinMode(enableX, OUTPUT);
  pinMode(enableY, OUTPUT);

  pinMode(sleepX, OUTPUT);
  pinMode(resetX, OUTPUT);

  pinMode(sleepY, OUTPUT);
  pinMode(resetY, OUTPUT);

  pinMode(ms1X, OUTPUT);
  pinMode(ms2X, OUTPUT);
  pinMode(ms3X, OUTPUT);

  pinMode(ms1Y, OUTPUT);
  pinMode(ms2Y, OUTPUT);
  pinMode(ms3Y, OUTPUT);

  // enable drivers
  digitalWrite(enableX, LOW);
  digitalWrite(enableY, LOW);

  digitalWrite(sleepX, HIGH);
  digitalWrite(resetX, HIGH);

  digitalWrite(sleepY, HIGH);
  digitalWrite(resetY, HIGH);

  // 1/16 microstepping
  digitalWrite(ms1X, HIGH);
  digitalWrite(ms2X, HIGH);
  digitalWrite(ms3X, HIGH);

  digitalWrite(ms1Y, HIGH);
  digitalWrite(ms2Y, HIGH);
  digitalWrite(ms3Y, HIGH);

  // 🔥 MUCH MORE POWER
  stepperX.setMaxSpeed(2000);
  stepperY.setMaxSpeed(2000);

  stepperX.setAcceleration(1500);
  stepperY.setAcceleration(1500);
}

void loop() {

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {

      int comma = input.indexOf(',');

      if (comma > 0) {
        int errorX = input.substring(0, comma).toInt();
        int errorY = input.substring(comma + 1).toInt();

        // --------------------
        // PROPORTIONAL CONTROL (power boost)
        // --------------------
        float speedX = errorX * 10;
        float speedY = errorY * 10;

        // limit (IMPORTANT for stability)
        speedX = constrain(speedX, -2000, 2000);
        speedY = constrain(speedY, -2000, 2000);

        // --------------------
        // OPTIONAL INVERT (FIX SPEJL)
        // --------------------
        if (invertX) speedX = -speedX;
        if (invertY) speedY = -speedY;

        stepperX.setSpeed(-speedX);
        stepperY.setSpeed(speedY);
      }

      input = "";

    } else {
      input += c;
    }
  }

  stepperX.runSpeed();
  stepperY.runSpeed();
}