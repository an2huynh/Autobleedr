#include "brakepress.h"
#include "syringe.h"
#include <Arduino.h>
#include <stdint.h>

void InitializeBrakePressPins(void) {
  // pinMode(SYR_DIR_PIN, OUTPUT);
  // pinMode(SYR_EN_PIN,  OUTPUT);
  // digitalWrite(SYR_EN_PIN, LOW);
}

bool HandBrakePress(void) {
  for (int i = 0; i < 4; ++i) { // test code that flashes lights
    digitalWrite(SYR_TEST_LED_PIN , HIGH);
    delay(500);                 // on for 200 ms
    digitalWrite(SYR_TEST_LED_PIN , LOW);
    delay(500);                 // off for 200 ms
  }

  // wait for STALLGUARD message from TMC2209, store the time it took to retrac

  // delay() for how long it took to retract

  return true;
}
