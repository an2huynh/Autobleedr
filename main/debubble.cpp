#include "debubble.h"
#include "syringe.h"
#include <stdint.h>
#include <Arduino.h>

int max_runs = 3;

void InitializeDebubblerPins() {
  pinMode(DEB_EN_PIN, OUTPUT);
}

bool DeBubbler(void) {
  int num_runs = 0;

  while (num_runs < max_runs) {
    analogWrite(DEB_EN_PIN, 1);
    delay(3000);
    analogWrite(DEB_EN_PIN, 0);
    delay(1500);
    num_runs += 1;
  }
  
  return true;
}
