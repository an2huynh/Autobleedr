#include "syringe.h"
#include <Arduino.h>

#define SYRINGE_PRESS_MS 8000

static inline unsigned long clampHz(int hz) { return hz <= 0 ? 1UL : (unsigned long)hz; }

void InitializeSyringePins(void) {
  pinMode(SYR_DIR_PIN, OUTPUT);
  pinMode(SYR_EN_PIN,  OUTPUT);
  digitalWrite(SYR_EN_PIN, LOW);
}

void syringeExtend(int ms) {
  digitalWrite(SYR_DIR_PIN, LOW);
  digitalWrite(SYR_EN_PIN,  HIGH);
  delay(ms);
  digitalWrite(SYR_EN_PIN,  LOW);
}

void syringeRetract(int ms) {
  digitalWrite(SYR_DIR_PIN, HIGH);
  digitalWrite(SYR_EN_PIN,  HIGH);
  delay(ms);
  digitalWrite(SYR_EN_PIN,  LOW);
}

void setEnable() {
  digitalWrite(SYR_EN_PIN,  HIGH);
}

void setExtend() {
  digitalWrite(SYR_DIR_PIN, LOW);
}

void setRetract() {
  digitalWrite(SYR_DIR_PIN, HIGH);
}

void setDisable() {
  digitalWrite(SYR_EN_PIN,  LOW);
}

// bool ActuateSyringe(void) {
//   syringeExtend(5000);
//   syringeContract(5000);
//   return true;
// }

bool ActuateSyringe(void) {
  syringeExtend(SYRINGE_PRESS_MS);
  syringeRetract(SYRINGE_PRESS_MS);   
  return true;
}