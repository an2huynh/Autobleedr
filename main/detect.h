#ifndef DETECT_H_
#define DETECT_H_

#include <stdbool.h>

// Initialize IR LED, ADC, and auto-calibrate baseline/noise
void DetectInit(void);

// Perform one detection step; returns current "detected" state
bool DetectStep(void);

// Wrapper used by FSM
bool ActuateSyringeCheckForBubbles(void);

#endif
