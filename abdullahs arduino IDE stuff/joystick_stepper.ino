#include <Stepper.h>

int stepsPerRevolution = 2048;

// Stepper on pins 6–9
Stepper myStepper(stepsPerRevolution, 6, 7, 8, 9);

int joystickPin = A0;

int deadZone = 50;   // prevents jitter around center

void setup() {
  // nothing else needed
}

void loop() {
  int joyValue = analogRead(joystickPin); // 0–1023

  int center = 512;
  int speed = joyValue - center;

  // Dead zone (stop when near center)
  if (abs(speed) < deadZone) {
    return;
  }

  // Map joystick deflection to motor speed
  int stepSpeed = map(abs(speed), 0, 512, 5, 20);

  myStepper.setSpeed(stepSpeed);

  if (speed > 0) {
    myStepper.step(10);   // forward
  } else {
    myStepper.step(-10);  // backward
  }
}