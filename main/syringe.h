#ifndef SYRINGE_H_
#define SYRINGE_H_

//GPIO Pins, 
#define SYR_DIR_PIN 18 //1=extend, 0=contract.
#define SYR_EN_PIN  19 //1=on, 0=off

// Test blink config
#define SYR_TEST_LED_PIN       2   // or a specific GPIO number
#define SYR_TEST_BLINK_HZ      5             // blink frequency
#define SYR_TEST_DURATION_MS   2000          // total duration

void InitializeSyringePins(void);
void syringeExtend(int ms);
void syringeRetract(int ms);

void setExtend(); // sets to extend/retract, WILL NOT STOP, must call setRetract() after
void setRetract();

void setEnable();
void setDisable();

bool ActuateSyringe(void);

#endif
