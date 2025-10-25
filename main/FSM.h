#ifndef INC_FSM_H_
#define INC_FSM_H_

#include <stdbool.h>

typedef enum {
  STATE_IDLE = 0,
  STATE_ACTUATE_AND_CHECK,
  STATE_BLEED_ACTUATE,
  STATE_BLEED_HAND_PRESS,
  STATE_BLEED_DEBUBBLE
} FSMState;

typedef struct {
  FSMState CurrentState;
  volatile bool startRequested;
} FSMType;

void InitializeFSM(FSMType *fsm);
void OutputFunction(FSMType *fsm);
void RequestStart(FSMType *fsm);

bool ActuateSyringe(void);
bool HandBrakePress(void);
bool DeBubbler(void);
bool ActuateSyringeCheckForBubbles(void);

#endif
