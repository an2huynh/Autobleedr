#include "brakepress.h"

// === Your original globals (kept) ===
uint16_t stop_condition = 66;   // default SGTHRS
bool     direction      = true; // true=extend, false=retract
uint32_t counter        = 0;    // step count from extend phase
bool     running        = false;

HardwareSerial& TMCSerial = Serial2;
TMC2209Stepper   driver{&TMCSerial, R_SENSE, DRIVER_ADDR};
hw_timer_t*      timer0   = nullptr;

int num_presses = 0;
int max_presses = 3;

void stepper_reset() {
  counter = 0;
  direction = true;

  digitalWrite(STEP, HIGH);
  digitalWrite(DIR, HIGH);

  digitalWrite(EN, HIGH);
  timerRestart(timer0);
}


void stepper_start() {
  digitalWrite(EN, LOW);

  timerRestart(timer0);
  timerStart(timer0);
}


void stepper_stop() {
  digitalWrite(DIR, LOW);
  direction = false;
  while (counter > 0);

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
    timerStop(timer0);
    running = false;
  } else {
    running = true;
  }
}


void interrupt() {
  direction = false;
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


void main_stepper_setup() {
  delay(1500); 
  Serial.begin(BAUD);
  stepper_setup();
}

void HandBrakePress() {
  num_presses = 0;

  while (num_presses < max_presses) {
    delay(100);
    if (!running) {
      //Serial.println("Stepper reset\n");
      running = true;
      stepper_reset();
      num_presses += 1;
      stepper_start();
    }
  }
}
