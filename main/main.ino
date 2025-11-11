#include <Arduino.h>
#include "FSM.h"
#include "syringe.h"
// #include "brakepress.h"
// #include "debubble.h"
// #include "detect.h"

static constexpr int PIN_START = 0; // boot pin, do not activate when EN pin is pressed
static FSMType g_fsm;

void IRAM_ATTR onStartRise() { RequestStart(&g_fsm); }

void setup() {
  InitializeSyringePins();

  pinMode(PIN_START, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_START), onStartRise, RISING);

  digitalWrite(2 , HIGH);

  InitializeFSM(&g_fsm);
}

void loop() {
  OutputFunction(&g_fsm);
}
