#include "FSM.h"
#include <Arduino.h>
#include "syringe.h"
#include "brakepress.h"
#include "debubble.h"
#include "detect.h"

bool light = false;

void InitializeFSM(FSMType *fsm) {
  // initialize light pin
  pinMode(LED_PIN, OUTPUT);

  if (!fsm) return;
  fsm->CurrentState   = STATE_IDLE;
  fsm->startRequested = false;
  fsm->abortRequested = false;
  fsm->stopAllowedAtMs  = 0;  
  fsm->noBubbleStreak = 0;
}

// If idle, request start; otherwise request abort-after-step.
void RequestStart(FSMType *fsm) {
  if (!fsm) return;
  if (fsm->CurrentState == STATE_IDLE) {
    Serial.print("Set Start\n");
    fsm->startRequested = true;
  } else {
    if (millis() < fsm->stopAllowedAtMs) {
      Serial.print("Stop ignored (<1s since start)\n");
      return;
    }
    Serial.print("Set Stop\n");
    fsm->abortRequested = true;
  }
}

static inline bool consumeAbort(FSMType *fsm) {
  if (fsm->abortRequested) {
    Serial.print("Consume Abort\n");
    fsm->abortRequested = false;
    fsm->CurrentState = STATE_IDLE;
    fsm->noBubbleStreak = 0;
    return true;
  }
  return false;
}

void OutputFunction(FSMType *fsm) {
  if (!fsm) return;

  switch (fsm->CurrentState) {
    case STATE_IDLE:
      if (fsm->startRequested) {
        
        digitalWrite(LED_PIN, LOW);
        Serial.print("Start\n");
        fsm->startRequested = false;
        DetectResetSession();
        fsm->noBubbleStreak = 0;
        fsm->stopAllowedAtMs = millis() + 1000;
        fsm->CurrentState = STATE_ACTUATE_AND_CHECK;   // or STATE_ACTUATE_AND_CHECK
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
      Serial.print("Bubbles? ");
      Serial.println(proceed);

      if (proceed) {
        // bubbles detected: keep going, reset "no bubble" streak
        fsm->noBubbleStreak = 0;
        fsm->CurrentState = STATE_BLEED_HAND_PRESS;
        digitalWrite(LED_PIN, HIGH);
      } else {
        // no bubbles this cycle: increment streak; stop only after 2 in a row
        fsm->noBubbleStreak++;
        digitalWrite(LED_PIN, LOW);

        if (fsm->noBubbleStreak >= 2) {
          fsm->CurrentState = STATE_IDLE;
        } else {
          // force at least one more cycle to confirm it's truly clean
          fsm->CurrentState = STATE_BLEED_HAND_PRESS;
        }
      }

    } break;

    default:
      fsm->CurrentState = STATE_IDLE;
      break;
  }
}
