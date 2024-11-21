#ifndef INTERRUPT_STEPPER_H
#define INTERRUPT_STEPPER_H

#include <DueTimer.h>

class InterruptStepper {
public:
  // The constctor where you need to manually provide an available timer.
  // There are 9 timers difined in the `DueTimer` library and they are 
  // `DueTimer::Timer1` - `DueTimer::Timer9`. You can also call the static
  // method `DueTimer::getAvailable()` to automatically get an available timer.
  InterruptStepper(DueTimer& timer, uint8_t step_pin);

  // User defined actions to perform every step.
  virtual void userUpdate();

  // Function which is called every step and returns the time period (in us)
  // to wait until the next step to happen. If the function returns 0, that 
  // means that the engine should stop.
  virtual uint32_t getNextInterval();

  // An interrupt function that performs the entire stepping logic.
  void stepInterrupt();

  // Make a step and begin the whole stepping logic.
  void start();

  // Attach interrupt to the Timer
  void attachInterrupt(void (*isr)());
  // Detach interrupt from the Timer
  void detachInterrupt();

private:
  // The timer from the `DueTimer` library which performs the `stepInterrupt()`
  // method that runs the stepper.
  DueTimer& _timer;
  // The stapper's step input pin. (Low to high transition means to step)
  const uint8_t _step_pin;

  uint32_t _start_time;
  uint32_t _step_time;
  uint32_t _next_interval;
  uint32_t _timer_period;
};

#endif