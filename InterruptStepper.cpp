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

InterruptStepper::InterruptStepper(DueTimer& timer, uint8_t step_pin) 
  : _timer(timer), _step_pin(step_pin) {}

// Trivial definition. Can be overwritten.
void InterruptStepper::userUpdate() {}

// Trivial definition. Can and should be overwritten.
uint32_t InterruptStepper::getNextInterval() { return 0; }

void InterruptStepper::stepInterrupt() {
  // Start measuring time
  _start_time = micros();

  digitalWrite(_step_pin, HIGH);

  userUpdate();
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

  // Calculate Timer period
  _timer_period = _next_interval - TIMER_SETUP_TIME - _step_time;

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

void InterruptStepper::start(uint32_t microseconds) {
  // Stop any `stepInterrupt` that might be already scheduled
  _timer.stop();
  // If the argument is 0 then we perform a step immidietaly and begin
  // the whole stepping logic
  if ( microseconds == 0 ) {
    return stepInterrupt();
  }
  // If the amount of time is less than the timer can support (which is 
  // timer's setup time + timer's min. period time) than we wait that time
  // using `delayMicroseconds()` function, after which we manually step and
  // start the whole stepping logic
  if ( microseconds <= TIMER_SETUP_TIME+MIN_PERIOD_TIME ) {
    delayMicroseconds(microseconds);
    return stepInterrupt();
  }
  // Otherwise just start the timer normally
  _timer.start(microseconds - TIMER_SETUP_TIME);
}

void InterruptStepper::attachInterrupt(void (*isr)()) {
  _timer.attachInterrupt(isr);
}

void InterruptStepper::detachInterrupt() {
  _timer.detachInterrupt();
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