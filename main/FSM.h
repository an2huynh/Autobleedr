#ifndef INC_FSM_H_
#define INC_FSM_H_

#include <stdbool.h>

#define LED_PIN   2

typedef enum {
  STATE_IDLE = 0,
  STATE_ACTUATE_AND_CHECK,
  STATE_BLEED_ACTUATE,
  STATE_BLEED_HAND_PRESS,
  STATE_BLEED_DEBUBBLE
} FSMState;

typedef struct {
  FSMState CurrentState;
  volatile bool startRequested;   // consumed in IDLE to start a cycle
  volatile bool abortRequested;   // set during any active state to go IDLE after step
  unsigned long stopAllowedAtMs;  
} FSMType;

void InitializeFSM(FSMType *fsm);
void OutputFunction(FSMType *fsm);
void RequestStart(FSMType *fsm);

#endif
