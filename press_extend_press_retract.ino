#include <SimpleFOC.h>

// Stepper motor & driver
StepperMotor motor = StepperMotor(10); 
StepperDriver4PWM driver = StepperDriver4PWM(6,7,8,9);

// Position (direction already fixed)
float outPosition = -500.0;
float target_position = 0.0;

// Button pin
int buttonPin = 3;

// Toggle state
bool isAtPositionB = false;

// Button state tracking
int lastButtonState = HIGH;

// Commander (optional)
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_position, cmd); }
void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }
void doVelocity(char* cmd) { command.scalar(&motor.velocity_limit, cmd); }

void setup() {

  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  pinMode(buttonPin, INPUT_PULLUP);

  driver.voltage_power_supply = 5;
  driver.voltage_limit = 12;

  if(!driver.init()){
    Serial.println("Driver init failed!");
    return;
  }

  motor.linkDriver(&driver);

  motor.voltage_limit = 12.0;
  motor.velocity_limit = 100.0;

  motor.controller = MotionControlType::angle_openloop;

  if(!motor.init()){
    Serial.println("Motor init failed!");
    return;
  }

  command.add('T', doTarget, "target angle");
  command.add('L', doLimit, "voltage limit");
  command.add('V', doVelocity, "movement velocity");

  Serial.println("Motor ready!");
}

void loop() {

  motor.loopFOC();

  int buttonState = digitalRead(buttonPin);

  // Detect button press (HIGH → LOW transition)
  if (buttonState == LOW && lastButtonState == HIGH) {
    
    // Toggle state
    isAtPositionB = !isAtPositionB;

    if (isAtPositionB) {
      target_position = outPosition;
    } else {
      target_position = 0.0;
    }

    delay(200); // debounce
  }

  lastButtonState = buttonState;

  motor.move(target_position);

  command.run();
}