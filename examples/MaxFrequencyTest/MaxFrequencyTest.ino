// MaxFrequencyTest.ino
//
// Run the motor at the maximum possible frequency ( i.e. the minimum
// possible period which is 25μs ).
//
// Results can be checked with an oscilloscope be measuring the `STEP_PIN` pin.

#include <InterruptStepper.h>

#define STEP_PIN 13
#define DIR_PIN 12

class MaxFreqTest : public InterruptStepper {
public:
  // Just invoke the base constructor
  MaxFreqTest(uint8_t step_pin, uint8_t direction_pin, 
                   DueTimer& timer, void (&update_func)())
    : InterruptStepper(step_pin, direction_pin, timer, update_func) {}

protected:
  // Always return the minimum possible interval ( which is 25μs )
  uint32_t getNextInterval() override {
    return 25;
  }
};

void updateFunc() {}

MaxFreqTest mft(STEP_PIN, DIR_PIN, Timer1, updateFunc);

void setup() {
  mft.attachInterrupt([](){ mft.stepInterrupt(); });
  mft.start(1000);
}

void loop() {}