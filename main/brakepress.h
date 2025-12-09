#ifndef BRAKEPRESS_H_
#define BRAKEPRESS_H_

#include <Arduino.h>
#include <stdint.h>
#include <TMCStepper.h>

#define RX2_PIN    16
#define TX2_PIN    17
#define BAUD       115200

#define DIR        18
#define STEP       19
#define DIAG       21
#define EN         23

#define R_SENSE    0.110f
#define DRIVER_ADDR 0b00

void stepper_reset(void);

void stepper_start(void);

void stepper_stop(void);

void step_timer(void);

void interrupt(void);

void stepper_setup(void);

void main_stepper_setup(void);

void HandBrakePress(void);

#endif
