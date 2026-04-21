#include <Arduino.h>
#include <AccelStepper.h>

// Define pins
const int dirPin = 2;
const int stepPin = 3;
const int sleepPin = 4;
const int resetPin = 5;
const int MS2 = 6;
const int MS1 = 7;
const int MS0 = 8;
const int enablePin = 9;

const int joyPinY = 14;
const int joyPinX = 15;
const int deadzone = 15; // arbritære tal

#define motorInterfaceType 1

AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);


void setup(){
  Serial.begin(9600);

  myStepper.setMaxSpeed(30000);
  myStepper.setAcceleration(800);
  myStepper.setSpeed(3000);
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  

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
  
}

void loop(){
  int joyValueX = analogRead(joyPinX);
  int joyValueY = analogRead(joyPinY);

  int center = 508;
  int speedX = joyValueX - center;
  int speedY = joyValueY - center;

  if(abs(speedX) > deadzone) {
    Serial.print("X: ");
    Serial.println(speedX);

    
    if(speedX > 0) {
      //myStepper.setSpeed(1000);
      myStepper.moveTo(myStepper.currentPosition()+1t00);
    } else {
     // myStepper.setSpeed(-1000);
     myStepper.moveTo(myStepper.currentPosition()-100);
    }
    
  } else{
    myStepper.moveTo(0);
  }

  myStepper.run();

  if(abs(speedY) > deadzone) {
    Serial.print("Y: ");
    Serial.println(speedY);
  }
  
}
