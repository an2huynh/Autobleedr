#include <TMCStepper.h>

#define RX2_PIN 16
#define TX2_PIN 17
#define BAUD    115200

#define PIN_15 15
#define PIN_2 2
#define PIN_25 25
#define PIN_26 26
#define PIN_33 33
#define DIR 18
#define STEP 19
#define DIAG 21
#define EN 23

#define R_SENSE    0.110f     // Adafruit TMC2209 sense resistor (110 mΩ)
#define DRIVER_ADDR 0b00


uint16_t stop_condition = 66;  // what to stop at
// should probably have a hard stop value here for number of loops
bool direction = true; // true is pulling, false is going back -> HIGH is clockwise, LOW is counter clockwise
uint32_t counter = 0; // counts how many steps to go back
bool running = false;  // Temp variable so we can reset the loop


hw_timer_t *timer0 = nullptr;
HardwareSerial& TMCSerial = Serial2;
TMC2209Stepper driver{&TMCSerial, R_SENSE, DRIVER_ADDR};


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


void setup() {
  delay(1500); 
  Serial.begin(BAUD);
  stepper_setup();
}

void loop() {
  // printf("%u\n", recent_sg);
  printf("Result: %u\n", driver.SG_RESULT());
  // printf("Threshold: %u\n", driver.SGTHRS());
  delay(100);
  if (!running) {
    Serial.println("Stepper reset\n");
    running = true;
    stepper_reset();
    stepper_start();
  }

  // if (!direction) {
  //   Serial.println("Interrupt");
  // }
}
  