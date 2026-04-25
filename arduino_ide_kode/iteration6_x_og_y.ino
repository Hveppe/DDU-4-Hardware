#include <AccelStepper.h>

// --------------------
// X AXIS PINS
// --------------------
const int stepPinX = 3;
const int dirPinX  = 2;
const int enableX  = 9;
const int sleepX   = 4;
const int resetX   = 5;
const int ms1X     = 8;
const int ms2X     = 7;
const int ms3X     = 6;

// --------------------
// Y AXIS PINS
// --------------------
const int stepPinY = A0;
const int dirPinY  = A1;
const int enableY  = 13;
const int sleepY   = A2;
const int resetY   = A3;
const int ms1Y     = 10;
const int ms2Y     = 11;
const int ms3Y     = 12;

// --------------------
// STEPPERS
// --------------------
AccelStepper stepperX(AccelStepper::DRIVER, stepPinX, dirPinX);
AccelStepper stepperY(AccelStepper::DRIVER, stepPinY, dirPinY);

// --------------------
// SETTINGS
// --------------------
int speedX = 0;
int speedY = 0;

// hvis retning er spejlvendt
bool invertX = false;
bool invertY = false;

// --------------------
// SETUP
// --------------------
void setup() {
  Serial.begin(115200);

  // OUTPUT PINS
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

  // ENABLE DRIVERS
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

  // STEPPER SETTINGS
  stepperX.setMaxSpeed(2000);
  stepperY.setMaxSpeed(2000);

  stepperX.setAcceleration(1500);
  stepperY.setAcceleration(1500);

  Serial.println("READY");
}

// --------------------
// LOOP
// --------------------
void loop() {

  // --------------------
  // SERIAL COMMANDS FROM PYTHON
  // --------------------
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {

      // ---------------- X AXIS ----------------

      case 'R': // right
        speedX = 1000;
        Serial.write("A", 1);
        break;

      case 'L': // left
        speedX = -1000;
        Serial.write("A", 1);
        break;

      case 'C': // center X stop
        speedX = 0;
        Serial.write("A", 1);
        break;

      case 'S': // search mode
        speedX = 600;
        Serial.write("A", 1);
        break;

      // ---------------- Y AXIS ----------------

      case 'U': // up
        speedY = -800;
        Serial.write("A", 1);
        break;

      case 'D': // down
        speedY = 800;
        Serial.write("A", 1);
        break;

      case 'Y': // center Y stop
        speedY = 0;
        Serial.write("A", 1);
        break;

      default:
        Serial.write("E", 1);
        break;
    }
  }

  // --------------------
  // OPTIONAL INVERT
  // --------------------
  int finalSpeedX = speedX;
  int finalSpeedY = speedY;

  if (invertX) finalSpeedX = -finalSpeedX;
  if (invertY) finalSpeedY = -finalSpeedY;

  // hvis X motor går forkert vej:
  stepperX.setSpeed(-finalSpeedX);

  // hvis Y motor går korrekt:
  stepperY.setSpeed(finalSpeedY);

  // RUN MOTORS
  stepperX.runSpeed();
  stepperY.runSpeed();
}