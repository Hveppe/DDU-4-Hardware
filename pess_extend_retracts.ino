#include <SimpleFOC.h>

// Stepper motor & driver
StepperMotor motor = StepperMotor(10);
StepperDriver4PWM driver = StepperDriver4PWM(6, 7, 8, 9);

// Positions and motion settings
float outPosition = -5000000.0;       // fully extended position (adjust as needed)
float target_position = 0.0;
float motorSpeed = 100.0;          // motor speed (adjust as needed)

// Timing for motion
unsigned long outTime = 15000;     // time to reach full extension (ms)
unsigned long backTime = 15000;    // time to return (ms)

// Button
int buttonPin = 3;
int lastButtonState = HIGH;

// Motor state
bool isRunning = false;
unsigned long startTime = 0;

// Commander for optional serial commands
Commander command = Commander(Serial);

void doTarget(char* cmd) { command.scalar(&target_position, cmd); }
void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }
void doVelocity(char* cmd) { command.scalar(&motor.velocity_limit, cmd); }

void setup() {
    Serial.begin(115200);
    SimpleFOCDebug::enable(&Serial);

    pinMode(buttonPin, INPUT_PULLUP);

    // Driver config
    driver.voltage_power_supply = 5;
    driver.voltage_limit = 12;

    if (!driver.init()) {
        Serial.println("Driver init failed!");
        return;
    }

    motor.linkDriver(&driver);
    motor.voltage_limit = 12.0;
    motor.velocity_limit = motorSpeed;
    motor.controller = MotionControlType::angle_openloop;

    if (!motor.init()) {
        Serial.println("Motor init failed!");
        return;
    }

    Serial.println("Motor ready!");

    // Optional serial commands
    command.add('T', doTarget, "target angle");
    command.add('L', doLimit, "voltage limit");
    command.add('V', doVelocity, "movement velocity");
}

void loop() {
    motor.loopFOC();

    int buttonState = digitalRead(buttonPin);

    // Detect single button press
    if (buttonState == LOW && lastButtonState == HIGH && !isRunning) {
        isRunning = true;
        startTime = millis();
    }

    lastButtonState = buttonState;

    // Motion sequence
    if (isRunning) {
        unsigned long elapsed = millis() - startTime;

        if (elapsed < outTime) {
            // Move OUT
            target_position = outPosition;
        } else if (elapsed < outTime + backTime) {
            // Move BACK
            target_position = 0.0;
        } else {
            // Done
            isRunning = false;
        }
    }

    motor.move(target_position);

    // Serial commands still available
    command.run();
}