#include <Arduino.h>

// Define pins
const int stepPin = 9;
const int dirPin = 8;
const int MS1 = 7;
const int MS2 = 6;
const int MS3 = 5;

const int joyPinY = 14;
const int joyPinX = 15;
const int deadzone = 15; // arbritære tal


void setup(){
  Serial.begin(9600);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  // Set direction og stepper size til 1/16
  digitalWrite(dirPin, HIGH);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);

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
      digitalWrite(dirPin, LOW);
    } else{
      digitalWrite(dirPin, HIGH);
    }

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(400);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(400);

  }

  if(abs(speedY) > deadzone) {
    Serial.print("Y: ");
    Serial.println(speedY);
  }
}
