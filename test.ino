#include "InterruptStepper.h"

#define LED_PIN 13

InterruptStepper stepper_1(Timer3, LED_PIN);

void interruptFunction() {
  stepper_1.stepInterrupt();
}

void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  stepper_1.attachInterrupt(interruptFunction);
}

void loop(){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  
  stepper_1.start();
  
  delay(1000); //wait 1 seconds
  
}
