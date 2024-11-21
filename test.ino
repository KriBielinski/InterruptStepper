#include <DueTimer.h>

#define LED_PIN 13

// Time it takes (in us) for the DueTimer to actually start counting time
#define TIMER_SETUP_TIME 8
// Maximum period time (in us) that the Timer can support
#define MAX_PERIOD_TIME 102261126

//#define TEST_DEBUG

void userUpdate() {

}

uint32_t getNextInterval() {
  static int counter = 0;
  counter++;
  if (counter > 10) {
    counter = 0;
    return 0;
  }
  return 20;
}

void stepInterrupt(){

  static uint32_t start_time = 0;
  static uint32_t step_time = 0;
  static uint32_t next_interval = 0;
  static uint32_t timer_period = 0;

  // Start measuring time
  start_time = micros();

  digitalWrite(LED_PIN, HIGH);

  userUpdate();
  next_interval = getNextInterval();

  digitalWrite(LED_PIN, LOW);

  // If stepper should stop
  if (next_interval == 0) {
    Timer3.stop();
    return;
  } 

  // Measure how long the stepper's step took
  // Calculation works correctly even if micros() overflows
  // Subtract 2us to compensate for how long measuring time itself took
  step_time = micros() - start_time - 2;

  // Calculate Timer period
  timer_period = next_interval - TIMER_SETUP_TIME - step_time;

  // Check if timer_period is less than 5us. Timer_period could
  // also overflow into very high values, so we also check for that
  if (timer_period < 5 || timer_period >= MAX_PERIOD_TIME) {
    timer_period = 5;
  }

  #if defined(TEST_DEBUG)
  printData(next_interval, step_time, timer_period);
  Timer3.stop();
  return;
  #endif

  // Single nop to make the step timing more accurate.
  // I belive that it helps because it compensates for the limited
  // resolution of the micros() function used to measure time above
  __NOP();

  Timer3.start(timer_period);
}

#if defined(TEST_DEBUG)
inline void printData(uint32_t next_interval, uint32_t step_time, uint32_t timer_period) {
  Serial.print("next_interval: "); Serial.println(next_interval);
  Serial.print("timer_setup_time: "); Serial.println(TIMER_SETUP_TIME);
  Serial.print("step_time: "); Serial.println(step_time);
  Serial.print("timer_period: "); Serial.println(timer_period);
  Serial.println();
}
#endif

void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  Timer3.attachInterrupt(stepInterrupt);
}

void loop(){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  
  Timer3.start(20);
  
  delay(1000); //wait 1 seconds
  
}
