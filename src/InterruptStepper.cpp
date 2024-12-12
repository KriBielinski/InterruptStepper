/*
  InterruptStepper.cpp - A stepper library designed for the Arduino Due board
  that can run a stepper motor(s) using timer interrupts.

  Copyright (C) 2024 Krzysztof Bieliński

  Licensed under GPLv3. For instructions and additional information go to
  https://github.com/KriBielinski/InterruptStepper
*/

#include "InterruptStepper.h"

// Time it takes (in μs) for the `DueTimer::Timer` to actually start counting time
#define TIMER_SETUP_TIME 8
// Minimum period (in μs) that the `DueTimer::Timer` can reliably operate on
#define MIN_PERIOD_TIME 5
// Maximum period time (in μs) that the `DueTimer::Timer` can support
#define MAX_PERIOD_TIME 102261126

InterruptStepper::InterruptStepper(PrecDueTimer& timer, void (&update_func)(), 
                  uint8_t interface, 
                  uint8_t pin1, 
                  uint8_t pin2, 
                  uint8_t pin3, 
                  uint8_t pin4, 
                  bool enable) 
  : AccelStepper(interface, pin1, pin2, pin3, pin4, enable), 
    _timer(timer), _update_func(update_func) {}

InterruptStepper::InterruptStepper(PrecDueTimer &timer, void (&update_func)(), 
                  void (*forward)(), void (*backward)())
  : AccelStepper(forward, backward), 
    _timer(timer), _update_func(update_func) {}


void InterruptStepper::stepInterrupt() {
  // Start measuring time
  _start_time = micros();
  
  //_timer.stop();

  // Step engine forward or backward depending on the stepper's direction
  _direction == DIRECTION_CW ? stepForward() : stepBackward();

  _update_func();
  _next_interval = getNextInterval();

  // If the stepper should stop
  if (_next_interval == 0) {
    _timer.stop();
    return;
  } 

  // Measure how long the stepper's step took
  // Subtract 2μs to compensate for how long measuring time itself took
  _step_time = micros() - _start_time - 2;

  start( _next_interval - _step_time );
}

void InterruptStepper::start(uint32_t interval) {
  // Calculate Timer period
  uint32_t _timer_period = interval - TIMER_SETUP_TIME;

  // Check if timer_period is less than MIN_PERIOD_TIME. Timer_period could
  // also underflow into very high values, so we also check for that
  if (_timer_period < MIN_PERIOD_TIME || _timer_period >= MAX_PERIOD_TIME) {
    _timer_period = MIN_PERIOD_TIME;
  }

  _timer.start(_timer_period);
}

void InterruptStepper::attachInterrupt(void (*isr)()) {
  _timer.attachInterrupt(isr);
}

void InterruptStepper::detachInterrupt() {
  _timer.detachInterrupt();
}

bool InterruptStepper::direction() {
  return _direction;
}

bool InterruptStepper::run() {
  return AccelStepper::isRunning();
}

void InterruptStepper::moveTo(long absolute) {
  if (_targetPos != absolute) {
    // Stop currently scheduled interrupts if max_speed needs to change
    _timer.stop();
    // Then perform calculations as normal
    _targetPos = absolute;
    computeNewSpeed();
    // compute new n?
  }
}

void InterruptStepper::move(long relative) {
  moveTo(_currentPos + relative);
}

void InterruptStepper::setMaxSpeed(float speed) {
  if (speed < 0.0)
    speed = -speed;
  if (_maxSpeed != speed) {
    // Stop currently scheduled interrupts if max_speed needs to change
    _timer.stop();
    // Then perform calculations as normal
    _maxSpeed = speed;
    _cmin = 1000000.0 / speed;
    // Recompute _n from current speed and adjust speed if accelerating or cruising
    if (_n > 0) {
      _n = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16
      //computeNewSpeed();
    }
    // Moved this line from above to here
    computeNewSpeed();
  }
}

void InterruptStepper::setAcceleration(float acceleration) {
  if (acceleration == 0.0)
	  return;
  if (acceleration < 0.0)
    acceleration = -acceleration;
  if (_acceleration != acceleration) {
    // Stop currently scheduled interrupts if max_speed needs to change
    _timer.stop();
    // Then perform calculations as normal
    // Recompute _n per Equation 17
    _n = _n * (_acceleration / acceleration);
    // New c0 per Equation 7, with correction per Equation 15
    _c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; // Equation 15
    _acceleration = acceleration;
    computeNewSpeed();
  }
}

void InterruptStepper::stop() {
  if (_speed != 0.0) {    
	  long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)) + 1; // Equation 16 (+integer rounding)
	  if (_speed > 0)
	    move(stepsToStop);
	  else
	    move(-stepsToStop);
  }
}

// Stop the timer and detach the interrupt if the object is destroyed or
// goes out of scope
InterruptStepper::~InterruptStepper() {
  _timer.stop();
  detachInterrupt();
}

uint32_t InterruptStepper::getNextInterval() { 
  return AccelStepper::computeNewSpeed();
}

uint32_t InterruptStepper::computeNewSpeed() {
  // Use the base method to compute the interval until the next step
  uint32_t interval = AccelStepper::computeNewSpeed();
  // How much time has passed already since the last step
  uint32_t time_since_step = micros() - _start_time;
  // We check whether the time since the last step is smaller than the interval.
  // If so then we wait an appropriate amount of time with the next step, 
  // otherwise we step immidietaly.
  time_since_step < interval ? start( interval - time_since_step ) : start();
  
  return interval;
}
