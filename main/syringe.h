#ifndef SYRINGE_H_
#define SYRINGE_H_

//GPIO Pins, 
#define SYR_DIR_PIN 26 //1=extend, 0=contract.
#define SYR_EN_PIN  27 //1=on, 0=off

#ifdef __cplusplus
extern "C" {
#endif

void InitializeSyringePins(void);
void syringeExtend(int ms);
void syringeContract(int ms);

bool ActuateSyringe(void);

#ifdef __cplusplus
}
#endif

#endif
