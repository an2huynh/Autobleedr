#ifndef DEBUBBLE_H_
#define DEBUBBLE_H_

#define DEB_DIR_PIN 33 // should be 1 all the time, direction
#define DEB_EN_PIN  32 //1=on, 0=off

#include <stdbool.h>
bool DeBubbler(void);
#endif
