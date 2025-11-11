#include "debubble.h"
#include "syringe.h"
#include <stdint.h>
#include <Arduino.h>

bool DeBubbler(void) {
  for (int i = 0; i < 4; ++i) {
    digitalWrite(SYR_TEST_LED_PIN , HIGH);
    delay(250);                 // on for 200 ms
    digitalWrite(SYR_TEST_LED_PIN , LOW);
    delay(250);                 // off for 200 ms
  }
  
  return true;
}
