#include "brakepress.h"

// === Your original globals (kept) ===
uint16_t stop_condition = 70;  // what to stop at
bool direction = true; // true is pulling, false is going back -> HIGH is clockwise, LOW is counter clockwise
uint32_t counter = 0; // counts how many steps to go back
bool running = false;  // What external files should be looking at
uint8_t presses = 0; // tracks how many squeezes we have


hw_timer_t *timer0 = nullptr;
HardwareSerial& TMCSerial = Serial2;
TMC2209Stepper driver{&TMCSerial, R_SENSE, DRIVER_ADDR};


void stepper_reset() {
  counter = 0;
  presses = 0;
  direction = true;
  running = false;

  digitalWrite(STEP, HIGH);
  digitalWrite(DIR, HIGH);

  digitalWrite(EN, HIGH);
  timerStop(timer0);
  timerRestart(timer0);
}


void stepper_start() {
  digitalWrite(EN, LOW);
  running = true;

  timerRestart(timer0);
  timerStart(timer0);
}


void stepper_stop() {
  digitalWrite(DIR, LOW);
  direction = false;
  presses = TARGET_PRESSES;
  while (counter > 0) {}

  stepper_reset();
}


void IRAM_ATTR step_timer() {
  if (direction || (!direction && counter > 0)) {
    digitalWrite(STEP, !digitalRead(STEP));
    if (direction)
      counter++;
    else
      counter--;
  }


  if (!direction && counter == 0) {
    if (presses == TARGET_PRESSES) {
      stepper_reset();
    } else {
      direction = true;
      digitalWrite(DIR, HIGH);
    }
  }
}


void interrupt() {
  direction = false;
  presses += 1;
  digitalWrite(DIR, LOW);
}


void stepper_setup() {
  pinMode(STEP, OUTPUT);
  pinMode(DIR,  OUTPUT);
  pinMode(EN,  OUTPUT);
  pinMode(DIAG, INPUT_PULLUP);

  digitalWrite(STEP, HIGH);
  digitalWrite(DIR, HIGH);
  digitalWrite(EN, HIGH);

  TMCSerial.begin(BAUD, SERIAL_8N1, RX2_PIN, TX2_PIN);

  driver.begin();
  driver.toff(5);
  // driver.senddelay(15); // Max out send delay to allow flushing of the buffer
  // driver.GSTAT(0b111);
  driver.rms_current(1500);               // mA
  driver.microsteps(8);
  driver.en_spreadCycle(false);
  driver.pwm_autoscale(true);  

  driver.TCOOLTHRS(0xFFFFF);
  driver.TPWMTHRS(0);

  driver.SGTHRS((uint8_t) (stop_condition / 2));
  // driver.SGTHRS(40);


  timer0 = timerBegin(1000000);              // 1 MHz
  timerAttachInterrupt(timer0, &step_timer); 
  timerAlarm(timer0, 500, true, 0); // 500 us

  attachInterrupt(digitalPinToInterrupt(DIAG), interrupt, HIGH);
}


void HandBrakePress() {
  stepper_reset();
  stepper_start();

  while (running) {
    // Serial.println(presses);
    delay(100);
  }
}
