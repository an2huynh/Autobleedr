#include "syringe.h"
#include <Arduino.h>

void InitializeSyringePins(void) {
  pinMode(SYR_DIR_PIN, OUTPUT);
  pinMode(SYR_EN_PIN,  OUTPUT);
  digitalWrite(SYR_EN_PIN, LOW);
}

void syringeExtend(int ms) {
  digitalWrite(SYR_DIR_PIN, HIGH);
  digitalWrite(SYR_EN_PIN,  HIGH);
  delay(ms);
  digitalWrite(SYR_EN_PIN,  LOW);
}

void syringeContract(int ms) {
  digitalWrite(SYR_DIR_PIN, LOW);
  digitalWrite(SYR_EN_PIN,  HIGH);
  delay(ms);
  digitalWrite(SYR_EN_PIN,  LOW);
}

bool ActuateSyringe(void) {
  syringeExtend(5000);
  syringeContract(5000);
  return true;
}
