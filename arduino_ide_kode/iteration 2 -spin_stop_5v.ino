// Stepper pins
int motorPins[4] = {8, 9, 10, 11};

// Step sequence
int stepSequence[4][4] = {
  {1, 0, 0, 1},
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0}
};

int stepIndex = 0;
bool spinning = false;

unsigned long lastStepTime = 0;
int stepDelay = 5; // speed

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }
}

void loop() {

  // --- READ SERIAL ---
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'S': // spin
        spinning = true;
        Serial.write("A", 1); // ACK
        break;

      case 'C': // stop
        spinning = false;
        stopMotor();
        Serial.write("A", 1); // ACK
        break;

      default:
        Serial.write("E", 1); // error
        break;
    }
  }

  // --- MOTOR LOOP ---
  if (spinning) {
    unsigned long currentTime = millis();

    if (currentTime - lastStepTime >= stepDelay) {
      stepMotor();
      lastStepTime = currentTime;
    }
  }
}

// Step motor
void stepMotor() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], stepSequence[stepIndex][i]);
  }

  stepIndex++;

  if (stepIndex >= 4) {
    stepIndex = 0;
  }
}

// Stop motor
void stopMotor() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], LOW);
  }
}