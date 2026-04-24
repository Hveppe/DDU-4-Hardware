#include <Arduino.h>
#include <AccelStepper.h>
#include <Stepper.h>

// Define pins
// 12V motor
const int dirPin = 2;
const int stepPin = 3;
const int sleepPin = 4;
const int resetPin = 5;
const int MS2 = 6;
const int MS1 = 7;
const int MS0 = 8;
const int enablePin = 9;

// 5V motor
const int IN1 = 10;
const int IN2 = 11;
const int IN3 = 12;
const int IN4 = 13;
const int stepsPerRevolution = 2048;

// Misc
const int joyPinY = 14;
const int joyPinX = 15;
const int buttonPin = 16;
const int relayPin = 17;

const int deadzone = 15; // arbritære tal

#define motorInterfaceType 1

AccelStepper myStepper12V(motorInterfaceType, stepPin, dirPin);
Stepper myStepper5V(stepsPerRevolution, IN1, IN2, IN3, IN4);


void setup(){
  Serial.begin(115200);

  myStepper12V.setMaxSpeed(1000);
  myStepper12V.setAcceleration(800);
  myStepper12V.setSpeed(1000);
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  

  // Set direction og stepper size
  //  MS1 | MS2 | MS3 | Microstep
  //  Low | Low | Low | Full
  // High | Low | Low | 1/2
  //  Low | High| Low | 1/4
  // High | High| Low | 1/8
  // High | High| High| 1/16

  digitalWrite(MS0, HIGH);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  /*
  digitalWrite(dirPin, HIGH);
  */
  digitalWrite(enablePin, LOW);
  digitalWrite(sleepPin, HIGH);
  digitalWrite(resetPin, HIGH);
  
  myStepper5V.setSpeed(18);
}

void loop(){
  int triggerState = Serial.read();
  if(triggerState == 0){
    digitalWrite(relayPin, HIGH);
    delay(1000);
    myStepper12V.moveTo(300);
    while(myStepper12V.currentPosition() < 300){
        myStepper12V.run();
    }
    digitalWrite(relayPin, LOW);
    myStepper12V.moveTo(0);
    while(myStepper12V.currentPosition() > 0){
        myStepper12V.run();
    }
    delay(200);
    myStepper5V.step(960);

  }

  myStepper12V.run();
}
