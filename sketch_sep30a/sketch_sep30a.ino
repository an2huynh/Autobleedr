#include <Arduino.h>
#include "FSM.h"

// Start pin (rising edge starts a cycle)
static constexpr int PIN_START = 34;

static FSMType g_fsm;

void IRAM_ATTR onStartRise() { RequestStart(&g_fsm); }

void setup() {
  pinMode(PIN_START, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_START), onStartRise, RISING);
  InitializeFSM(&g_fsm);
}

void loop() {
  OutputFunction(&g_fsm);
}
