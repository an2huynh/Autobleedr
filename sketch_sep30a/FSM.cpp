#include "FSM.h"
#include <Arduino.h>

void InitializeFSM(FSMType *fsm) {
  if (!fsm) return;
  fsm->CurrentState = STATE_IDLE;
  fsm->startRequested = false;
}

void RequestStart(FSMType *fsm) {
  if (!fsm) return;
  fsm->startRequested = true;
}

void OutputFunction(FSMType *fsm) {
  if (!fsm) return;

  switch (fsm->CurrentState) {
    case STATE_IDLE:
      if (fsm->startRequested) {
        fsm->startRequested = false;
        fsm->CurrentState = STATE_ACTUATE_AND_CHECK;
      }
      break;

    case STATE_ACTUATE_AND_CHECK: {
      bool proceed = ActuateSyringeCheckForBubbles();
      fsm->CurrentState = proceed ? STATE_BLEED_ACTUATE : STATE_IDLE;
    } break;

    case STATE_BLEED_ACTUATE:
      (void)ActuateSyringe();
      fsm->CurrentState = STATE_BLEED_HAND_PRESS;
      break;

    case STATE_BLEED_HAND_PRESS:
      (void)HandBrakePress();
      fsm->CurrentState = STATE_BLEED_DEBUBBLE;
      break;

    case STATE_BLEED_DEBUBBLE:
      (void)DeBubbler();
      fsm->CurrentState = STATE_ACTUATE_AND_CHECK;
      break;

    default:
      fsm->CurrentState = STATE_IDLE;
      break;
  }
}
