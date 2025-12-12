#ifndef SYRINGE_H_
#define SYRINGE_H_

//GPIO Pins, 
#define SYR_DIR_PIN 25 //1=extend, 0=contract.
#define SYR_EN_PIN  33 //1=on, 0=off

void InitializeSyringePins(void);
void syringeExtend(int ms);
void syringeRetract(int ms);

void setExtend(); // sets to extend/retract, WILL NOT STOP, must call setRetract() after
void setRetract();

void setEnable();
void setDisable();

bool ActuateSyringe(void);

#endif
