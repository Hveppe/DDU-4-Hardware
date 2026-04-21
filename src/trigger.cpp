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
const int buttonPin = 16;
const int relayPin = 17;

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
  
}

void loop(){
  int buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  
  if(buttonState == 0){
    digitalWrite(relayPin, HIGH);
    myStepper.moveTo(300);
    while(myStepper.currentPosition() < 300){
        myStepper.run();
    }
  }else{
    digitalWrite(relayPin, LOW);
    myStepper.moveTo(0);
    while(myStepper.currentPosition() > 0){
        myStepper.run();
    }
  }

  myStepper.run();
}


 