// Minimal.ino
//
// A minimal working setup required for the InterruptStepper to work.

#include <InterruptStepper.h>

// Define pins that will be used to step the motor and that will decide the
// direction in which motor will rotate
#define STEP_PIN 13
#define DIR_PIN 12

// Define an update function that will be passed to the stepper. Inside you
// can place all the code that you wish to run each time the motor steps.
void updateFunc() {}

// Create the object that will run the stepper motor by passing to it the pins
// that will run the motor, a unique timer for timing purposes and the update
// function declared previously.
InterruptStepper stepper(STEP_PIN, DIR_PIN, Timer2, updateFunc);

void setup() {
  // A setup function necessary for the stepper to work
  stepper.attachInterrupt([](){ stepper.stepInterrupt(); });
}

void loop() {
  //
  // All the code to run the stepper goes here.
  //
}