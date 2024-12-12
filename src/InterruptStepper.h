/*
  InterruptStepper.h - A stepper library designed for the Arduino Due board
  that can run a stepper motor(s) using timer interrupts.

  Copyright (C) 2024 Krzysztof Bieliński

  Licensed under GPLv3. For instructions and additional information go to
  https://github.com/KriBielinski/InterruptStepper
*/

#ifndef INTERRUPT_STEPPER_H
#define INTERRUPT_STEPPER_H

#include <PrecDueTimer.h>
#include "AccelStepper/AccelStepper.h"

class InterruptStepper : public AccelStepper {
public:
  // The constructor where you need to manually provide an available timer.
  // There are 9 timers defined in the `DueTimer` library and they are 
  // `DueTimer::Timer0` to `DueTimer::Timer8`. You can also call the static
  // method `DueTimer::getAvailable()` to automatically get an available timer.
  InterruptStepper(PrecDueTimer& timer, void (&update_func)(), 
                  uint8_t interface = InterruptStepper::FULL4WIRE, 
                  uint8_t pin1 = 2, 
                  uint8_t pin2 = 3, 
                  uint8_t pin3 = 4, 
                  uint8_t pin4 = 5, 
                  bool enable = true);
  
  // The constructor in which you can provide your own implementation of
  // forward and backward steps.
  InterruptStepper(PrecDueTimer& timer, void (&update_func)(),
                  void (*forward)(), void (*backward)());

  // An interrupt function that performs the entire stepping logic.
  void stepInterrupt();

  // Make a step and begin the whole stepping logic, after the specified
  // interval (in μs)
  void start(uint32_t interval = 0);

  // Attach interrupt to the Timer
  void attachInterrupt(void (*isr)());
  // Detach interrupt from the Timer
  void detachInterrupt();

  // Returnes the direction the motor is currently spinning in. Value of 1
  // means clockwise. You can also use `AccelStepper::Direction` enum to
  // compare the output of this method to. If the motor is stationary, then
  // the output of this method is undefined.
  bool direction();

  // Method overridden from the AccelStepper library to make sure that it
  // doesn't interfere with the motor when the user accidentally calls this
  // method.
  bool run();

  // Below are methods overriden from the AccelStepper library that need to
  // stop currently scheduled interrupts before doing their own calculations
  // so that no race conditions occur.

  void moveTo(long absolute);
  void move(long relative);
  void setMaxSpeed(float speed);
  void setAcceleration(float acceleration);
  void stop();

  ~InterruptStepper();

protected:
  // The timer from the `DueTimer` library which performs the `stepInterrupt()`
  // method that runs the stepper.
  PrecDueTimer& _timer;
  // A function passed by the user that will be run every step (every time
  // the timer runs a `stepInterrupt()` method).
  void (&_update_func)();

  // Method which is called every step and returns the time period (in μs)
  // to wait until the next step should occur. If the method returns 0, that 
  // means that the engine should stop.
  virtual uint32_t getNextInterval();

  // Method overriden from the `AccelStepper` class to allow the use of the
  // interrupt capabilities of this class. This method calculates how much
  // time to wait until the next step is due.
  uint32_t computeNewSpeed() override;

private:
  // Time at which the last step occured
  uint32_t _start_time = 0;
  // How long the step itself took
  uint32_t _step_time;
  // The interval until the next step is due
  uint32_t _next_interval;
};

#endif