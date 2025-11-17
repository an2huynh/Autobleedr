#include "FSM.h"
#include <Arduino.h>
#include "syringe.h"
#include "brakepress.h"
#include "debubble.h"
#include "detect.h"

void InitializeFSM(FSMType *fsm) {
  if (!fsm) return;
  fsm->CurrentState   = STATE_IDLE;
  fsm->startRequested = false;
  fsm->abortRequested = false;
}

// If idle, request start; otherwise request abort-after-step.
void RequestStart(FSMType *fsm) {
  if (!fsm) return;
  if (fsm->CurrentState == STATE_IDLE) {
    fsm->startRequested = true;
  } else {
    fsm->abortRequested = true;
  }
}

static inline bool consumeAbort(FSMType *fsm) {
  if (fsm->abortRequested) {
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
        fsm->startRequested = false;
        fsm->CurrentState = STATE_BLEED_ACTUATE;   // or STATE_ACTUATE_AND_CHECK
      }
      break;

    case STATE_BLEED_ACTUATE:
      (void)ActuateSyringe();
      if (consumeAbort(fsm)) break;
      fsm->CurrentState = STATE_BLEED_HAND_PRESS;
      break;

    case STATE_BLEED_HAND_PRESS:
      (void)HandBrakePress();
      if (consumeAbort(fsm)) break;
      fsm->CurrentState = STATE_BLEED_DEBUBBLE;
      break;

    case STATE_BLEED_DEBUBBLE:
      (void)DeBubbler();
      if (consumeAbort(fsm)) break;
      fsm->CurrentState = STATE_ACTUATE_AND_CHECK;
      break;

    case STATE_ACTUATE_AND_CHECK: {
      bool proceed = ActuateSyringeCheckForBubbles();
      if (consumeAbort(fsm)) break;
      fsm->CurrentState = proceed ? STATE_BLEED_ACTUATE : STATE_IDLE;
    } break;

    default:
      fsm->CurrentState = STATE_IDLE;
      break;
  }
}
