#ifndef INTERRUPT_STEPPER_H
#define INTERRUPT_STEPPER_H

#include <DueTimer.h>

class InterruptStepper {
public:
  // The constructor where you need to manually provide an available timer.
  // There are 9 timers defined in the `DueTimer` library and they are 
  // `DueTimer::Timer0` to `DueTimer::Timer8`. You can also call the static
  // method `DueTimer::getAvailable()` to automatically get an available timer.
  InterruptStepper(uint8_t step_pin, DueTimer& timer, void (&update_func)());

  // Function which is called every step and returns the time period (in us)
  // to wait until the next step to happen. If the function returns 0, that 
  // means that the engine should stop.
  virtual uint32_t getNextInterval();

  // An interrupt function that performs the entire stepping logic.
  void stepInterrupt();

  // Make a step and begin the whole stepping logic, after the specified
  // interval (in us)
  void start(uint32_t interval = 0);

  // Stop the engine and the whole stepping logic.
  void stop();

  // Attach interrupt to the Timer
  void attachInterrupt(void (*isr)());
  // Detach interrupt from the Timer
  void detachInterrupt();

  ~InterruptStepper();

protected:
  // The stapper's step input pin. (Low to high transition means to step)
  const uint8_t _step_pin;
  // The timer from the `DueTimer` library which performs the `stepInterrupt()`
  // method that runs the stepper.
  DueTimer& _timer;
  // A function passed by the user that will be run every step (every time
  // the timer runs a `stepInterrupt()` method).
  void (&_update_func)();

private:
  uint32_t _start_time;
  uint32_t _step_time;
  uint32_t _next_interval;
};

#endif