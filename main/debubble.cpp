#include "debubble.h"
#include "syringe.h"
#include <stdint.h>
#include <Arduino.h>

void InitializeDebubblerPins() {
  analogWrite(DEB_EN_PIN, 0);  
  analogWriteResolution(DEB_EN_PIN, 8);     // duty range 0..255
  analogWriteFrequency(DEB_EN_PIN, 1000);   // 1 kHz PWM
}

bool DeBubbler(void) {
  int num_runs = 0;

  analogWrite(DEB_EN_PIN, 127);
  delay(3000);
  analogWrite(DEB_EN_PIN, 0);
  
  return true;
}
