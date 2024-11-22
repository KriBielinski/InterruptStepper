#include "InterruptStepper.h"

#define LED_PIN 13

InterruptStepper stepper(LED_PIN, Timer3);

void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  stepper.attachInterrupt([](){ stepper.stepInterrupt(); });
}

void loop(){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  
  stepper.start();
  
  delay(1000); //wait 1 seconds
  
}
