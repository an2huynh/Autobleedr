#include "FSM.h"
#include <Arduino.h>
#include "syringe.h"
#include "brakepress.h"
#include "debubble.h"
#include "detect.h"

bool light = false;

void ToggleLight() {
  // for testing purposes, we toggle light
  if (light) {
    digitalWrite(LED_PIN, LOW);
    light = false;
  } else {
    digitalWrite(LED_PIN, HIGH);
    light = true;
  }
}

void InitializeFSM(FSMType *fsm) {
  // initialize light pin
  pinMode(LED_PIN, OUTPUT);

  if (!fsm) return;
  fsm->CurrentState   = STATE_IDLE;
  fsm->startRequested = false;
  fsm->abortRequested = false;
}

// If idle, request start; otherwise request abort-after-step.
void RequestStart(FSMType *fsm) {
  if (!fsm) return;
  if (fsm->CurrentState == STATE_IDLE) {
    Serial.print("Set Start\n");
    fsm->startRequested = true;
  } else {
    Serial.print("Set Stop\n");
    //fsm->abortRequested = true;
  }
}

static inline bool consumeAbort(FSMType *fsm) {
  if (fsm->abortRequested) {
    Serial.print("Consume Abort\n");
    fsm->abortRequested = false;
    fsm->CurrentState = STATE_IDLE;
    return true;
  }
  return false;
}

void OutputFunction(FSMType *fsm) {
  if (!fsm) return;

  switch (fsm->CurrentState) {
    case STATE_IDLE:
      if (fsm->startRequested) {
        Serial.print("Start\n");
        fsm->startRequested = false;
        fsm->CurrentState = STATE_BLEED_ACTUATE;   // or STATE_ACTUATE_AND_CHECK
      }
      break;

    case STATE_BLEED_ACTUATE:
      Serial.println("Actuate");
      (void)ActuateSyringe();
      if (consumeAbort(fsm)) {
        Serial.print("Stop\n");
        break;
      }
      fsm->CurrentState = STATE_BLEED_HAND_PRESS;
      break;

    case STATE_BLEED_HAND_PRESS:
      Serial.println("Hand Press");
      (void)HandBrakePress();
      if (consumeAbort(fsm)) {
        Serial.print("Stop\n");
        break;
      }
      fsm->CurrentState = STATE_BLEED_DEBUBBLE;
      break;

    case STATE_BLEED_DEBUBBLE:
      Serial.println("DeBubble");
      (void)DeBubbler();
      if (consumeAbort(fsm)) {
        Serial.print("Stop\n");
        break;
      }
      fsm->CurrentState = STATE_ACTUATE_AND_CHECK;
      break;

    case STATE_ACTUATE_AND_CHECK: {
      Serial.println("Bubble check");
      bool proceed = ActuateSyringeCheckForBubbles();
      if (consumeAbort(fsm)) {
        Serial.print("Stop\n");
        break;
      }
      Serial.print("Continuing? ");
      Serial.println(proceed);
      fsm->CurrentState = proceed ? STATE_BLEED_ACTUATE : STATE_IDLE;
    } break;

    default:
      fsm->CurrentState = STATE_IDLE;
      break;
  }
}
