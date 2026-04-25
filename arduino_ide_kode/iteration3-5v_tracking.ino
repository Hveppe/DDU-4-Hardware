// --------------------
// STEPPER SETUP
// --------------------

// Skift rækkefølgen hvis den går forkert
int motorPins[4] = {8, 9, 10, 11};

// Simpel og stabil full-step sequence
int stepSequence[4][4] = {
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};

int stepIndex = 0;

bool spinning = false;
int direction = 1; // 1 = højre, -1 = venstre

unsigned long lastStepTime = 0;
int stepDelay = 5; // lavere = hurtigere


// --------------------
// SETUP
// --------------------
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }
}


// --------------------
// LOOP
// --------------------
void loop() {

  // --- SERIAL COMMANDS ---
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {

      case 'S': // sweep (default højre)
        spinning = true;
        direction = 1;
        Serial.write("A", 1);
        break;

      case 'C': // stop
        spinning = false;
        stopMotor();
        Serial.write("A", 1);
        break;

      case 'L': // venstre
        spinning = true;
        direction = -1;
        Serial.write("A", 1);
        break;

      case 'R': // højre
        spinning = true;
        direction = 1;
        Serial.write("A", 1);
        break;

      default:
        Serial.write("E", 1);
        break;
    }
  }

  // --- MOTOR CONTROL ---
  if (spinning) {
    unsigned long currentTime = millis();

    if (currentTime - lastStepTime >= stepDelay) {
      stepMotor();
      lastStepTime = currentTime;
    }
  }
}


// --------------------
// STEP MOTOR (RETNING FIX)
// --------------------
void stepMotor() {
  stepIndex += direction;

  if (stepIndex >= 4) stepIndex = 0;
  if (stepIndex < 0) stepIndex = 3;

  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], stepSequence[stepIndex][i]);
  }
}


// --------------------
// STOP MOTOR
// --------------------
void stopMotor() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], LOW);
  }
}