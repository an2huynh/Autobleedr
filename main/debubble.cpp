#include "debubble.h"
#include "syringe.h"
#include <stdint.h>
#include <Arduino.h>

int max_runs = 0;

void InitializeDebubblerPins() {
  analogWriteResolution(DEB_EN_PIN, 8);     // duty range 0..255
  analogWriteFrequency(DEB_EN_PIN, 1000);   // 1 kHz PWM
  analogWrite(DEB_EN_PIN, 127);  
}

bool DeBubbler(void) {
  int num_runs = 0;

  while (num_runs < max_runs) {
    for (int d = 0; d <= 255; d++) { analogWrite(DEB_EN_PIN, d); delay(5); }
    for (int d = 255; d >= 0; d--) { analogWrite(DEB_EN_PIN, d); delay(5); }
    num_runs += 1;
  }
  
  return true;
}
