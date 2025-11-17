#ifndef BRAKEPRESS_H_
#define BRAKEPRESS_H_
#include <stdbool.h>
bool HandBrakePress(void);
void InitializeBrakePressPins(void);
void retract(void);
void extend(void);
#endif
