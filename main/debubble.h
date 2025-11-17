#ifndef DEBUBBLE_H_
#define DEBUBBLE_H_

#define DEB_DIR_PIN 22 // should be 1 all the time, direction
#define DEB_EN_PIN  23 //1=on, 0=off

void InitializeDebubblerPins(void);

bool DeBubbler(void);

#endif
