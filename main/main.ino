#include <Arduino.h>
#include "FSM.h"
#include "syringe.h"
#include "brakepress.h"
#include "debubble.h"
#include "detect.h"

static constexpr int PIN_START = 0; // boot pin, do not press when EN pin is pressed
static FSMType g_fsm;

void IRAM_ATTR onStartRise() { RequestStart(&g_fsm); }

void setup() {
  Serial.begin(9600);
  delay(600);
  Serial.println("Setup");
  InitializeSyringePins();
  stepper_setup();
  DetectInit();
  InitializeDebubblerPins();

  // attach boot pin to start/stop
  pinMode(PIN_START, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_START), onStartRise, RISING);

  InitializeFSM(&g_fsm);
}

void loop() {
  //ToggleLight();
  OutputFunction(&g_fsm);
}
