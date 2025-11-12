#include "debubble.h"
#include "syringe.h"
#include <stdint.h>
#include <Arduino.h>

const int pwm_pin = 26;
const int dir_pin = 27;
const int enable_pin = 14;

void InitializeDebubblerPins() {
  analogWriteResolution(pwm_pin, 8);     // duty range 0..255
  analogWriteFrequency(pwm_pin, 1000);   // 1 kHz PWM
  analogWrite(pwm_pin, 127);        // ~5% duty on pwm_pin
  pinMode(dir_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
  digitalWrite(dir_pin, HIGH);
  digitalWrite(enable_pin, HIGH);
}

bool DeBubbler(void) {
  analogWrite(pwm_pin, 160); delay(3000);
  analogWrite(pwm_pin, 0);
}
  
  return true;
}
