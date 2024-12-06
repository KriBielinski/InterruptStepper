// Multiple.ino
//
// Running multiple steppers with interrupts simultaneously

#include <InterruptStepper.h>

// Define pins that will be used to run each stepper
const uint8_t step_pin_1 = 13; // Stepper 1
const uint8_t dir_pin_1 = 12;
const uint8_t step_pin_2 = 11; // Stepper 2
const uint8_t dir_pin_2 = 10;
const uint8_t step_pin_3 = 9; // Stepper 3
const uint8_t dir_pin_3 = 8;


// Define an update function for each stepper. Inside you
// can place all the code that you wish to run each time the appropriate
// stepper steps.
void updateFunc_1() {
  //
  // Code that will run each time stepper_1 steps
  //
}
void updateFunc_2() {
  //
  // Code that will run each time stepper_2 steps
  //
}
void updateFunc_3() {
  //
  // Code that will run each time stepper_3 steps
  //
}

// For multiple steppers to be run independently from each other they each need
// to use a unique timer. There are 9 unique timers defined: 
// Timer0, Timer1, Timer2, ... Timer7, Timer8
InterruptStepper stepper_1(Timer1, updateFunc_1, InterruptStepper::DRIVER, step_pin_1, dir_pin_1);
InterruptStepper stepper_2(Timer2, updateFunc_2, InterruptStepper::DRIVER, step_pin_2, dir_pin_2);
InterruptStepper stepper_3(Timer3, updateFunc_3, InterruptStepper::DRIVER, step_pin_3, dir_pin_3);

void setup() {
  // A setup function needs to be called for every stepper defined
  stepper_1.attachInterrupt([](){ stepper_1.stepInterrupt(); });
  stepper_2.attachInterrupt([](){ stepper_2.stepInterrupt(); });
  stepper_3.attachInterrupt([](){ stepper_3.stepInterrupt(); });
}

void loop() {
  //
  // All the code to run the steppers goes here.
  //
}