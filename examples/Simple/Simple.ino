// Simple.ino
//
// A simple example that shows how to set up the InterruptStepper
// and how to use it. 

#include <InterruptStepper.h>

#define STEP_PIN 13
#define DIR_PIN 12

void updateFunc() {}

InterruptStepper stepper(Timer3, updateFunc, InterruptStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  Serial.begin(9600);

  stepper.attachInterrupt([](){ stepper.stepInterrupt(); });

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(50);
}

void loop() {
  // Move the stepper constantly between the positions 0 and 3000
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

  // The stepper will work correctly even if we put delay in the main loop
  delay(1000);

  Serial.println(stepper.currentPosition());
  Serial.println(stepper.direction());
}
