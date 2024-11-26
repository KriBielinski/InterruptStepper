#include "InterruptStepper.h"

#define STEP_PIN 13
#define DIR_PIN 12

void updateFunc() {}

InterruptStepper stepper(STEP_PIN, DIR_PIN, Timer3, updateFunc);

void setup(){
  pinMode(STEP_PIN, OUTPUT);
  Serial.begin(9600);

  stepper.attachInterrupt([](){ stepper.stepInterrupt(); });

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(50);
}

void loop(){

  
  
  if(stepper.distanceToGo()==0) {
    if(stepper.currentPosition() >= 3000) {
      stepper.moveTo(0);
      Serial.println("Move to 0");
    }
    else {
      stepper.moveTo(3000);
      Serial.println("Move to 3000");
    }
  }

delay(1000);

Serial.println(stepper.currentPosition());
Serial.println(stepper.direction());

  
  
}
