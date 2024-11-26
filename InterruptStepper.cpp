#include "InterruptStepper.h"

// Get debugging information to the Serial output defined in 
// INTERRUPT_STEPPER_SERIAL. Note that it will
// interfere with the correct stepping of the InterruptStepper. Intended 
// only as a developer tool.
// #define INTERRUPT_STEPPER_DEBUG
// #define INTERRUPT_STEPPER_SERIAL Serial


// Time it takes (in us) for the `DueTimer::Timer` to actually start counting time
#define TIMER_SETUP_TIME 8
// Minimum period (in us) that the `DueTimer::Timer` can reliably operate on
#define MIN_PERIOD_TIME 5
// Maximum period time (in us) that the `DueTimer::Timer` can support
#define MAX_PERIOD_TIME 102261126

InterruptStepper::InterruptStepper(uint8_t step_pin, uint8_t direction_pin, 
                                  DueTimer& timer, void (&update_func)()) 
  : AccelStepper(AccelStepper::DRIVER, step_pin, direction_pin), 
    _step_pin(step_pin), _dir_pin(direction_pin), _timer(timer), _update_func(update_func) {}


void InterruptStepper::stepInterrupt() {
  // Start measuring time
  _start_time = micros();

  digitalWrite(_dir_pin, _direction);
  
  digitalWrite(_step_pin, HIGH);

  _direction == DIRECTION_CW ? _currentPos += 1 : _currentPos -= 1;

  _update_func();
  _next_interval = getNextInterval();

  digitalWrite(_step_pin, LOW);

  // If stepper should stop
  if (_next_interval == 0) {
    _timer.stop();
    return;
  } 

  // Measure how long the stepper's step took
  // Calculation works correctly even if micros() overflows
  // Subtract 2us to compensate for how long measuring time itself took
  _step_time = micros() - _start_time - 2;

  start( _next_interval - _step_time );
}

void InterruptStepper::start(uint32_t interval) {
  // Calculate Timer period
  uint32_t _timer_period = interval - TIMER_SETUP_TIME;

  // Check if timer_period is less than MIN_PERIOD_TIME. Timer_period could
  // also overflow into very high values, so we also check for that
  if (_timer_period < MIN_PERIOD_TIME || _timer_period >= MAX_PERIOD_TIME) {
    _timer_period = MIN_PERIOD_TIME;
  }

  #if defined(INTERRUPT_STEPPER_DEBUG)
  printData(_next_interval, _step_time, _timer_period);
  _timer.stop();
  return;
  #endif

  _timer.start(_timer_period);
}

void InterruptStepper::stop() {
  _timer.stop();
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
  // If so then we wait with the next an appropriate amount if time, otherwise
  // we step immidietaly.
  time_since_step < interval ? start( interval - time_since_step ) : start();
  
  return interval;
}

#if defined(INTERRUPT_STEPPER_DEBUG)
inline void printData(uint32_t next_interval, uint32_t step_time, uint32_t timer_period) {
  INTERRUPT_STEPPER_SERIAL.print("next_interval: ");
  INTERRUPT_STEPPER_SERIAL.println(next_interval);
  INTERRUPT_STEPPER_SERIAL.print("timer_setup_time: ");
  INTERRUPT_STEPPER_SERIAL.println(TIMER_SETUP_TIME);
  INTERRUPT_STEPPER_SERIAL.print("step_time: ");
  INTERRUPT_STEPPER_SERIAL.println(step_time);
  INTERRUPT_STEPPER_SERIAL.print("timer_period: ");
  INTERRUPT_STEPPER_SERIAL.println(timer_period);
  INTERRUPT_STEPPER_SERIAL.println();
}
#endif