#include "detect.h"
#include "syringe.h"
#include <Arduino.h>
#include <driver/adc.h>
#include <math.h>

// -------------------- PINS --------------------
#define IR_LED_PIN        32
#define SENSOR_PIN        35
#define STATUS_LED_PIN     2

// -------------------- TUNABLES --------------------
static const uint8_t  ADC_BITS               = 12;
static const adc_attenuation_t ADC_ATTEN     = ADC_11db;
static const uint16_t LED_SETTLE_US          = 50;
static const uint8_t  SAMPLES_PER_PHASE      = 2;
static const uint16_t CAL_TIME_MS            = 3000;

// Relative change: 0.10 = 10%
static const float    MIN_RELATIVE_DELTA     = 0.1f;

// Noise/absolute safety floors
// static const float    THRESH_NOISE_MULT      = 4.0f;
// static const uint16_t THRESH_MIN_MARGIN      = 40;
static const uint16_t HYSTERESIS_COUNTS      = 20;
static const uint8_t  DEBOUNCE_HITS_REQUIRED = 1;
static const uint8_t  DEBOUNCE_CLEAR_REQUIRED= 3;
static const uint16_t LOOP_PERIOD_MS         = 1;

// true  -> detect RISE from baseline
// false -> detect DROP from baseline
// static const bool     DETECT_RISE            = true;

// -------------------- RUNTIME STATE --------------------
static float    baseline = 0.0f;
static float    noiseStd = 5.0f;
static float threshUp = 0.0f;
static float threshDn = 0.0f;
static bool     detected = false;
static uint8_t  hitCount = 0;
static uint8_t  clearCount = 0;

static const float EMA_ALPHA = 0.25f;
static float emaDiff = 0.0f;

// -------------------- HELPERS --------------------
static uint16_t readADC() {
  return analogRead(SENSOR_PIN);
}

// Ambient-cancelled signal: avg(ON) - avg(OFF)
static int16_t readDifferenced() {
  digitalWrite(IR_LED_PIN, HIGH);
  delayMicroseconds(LED_SETTLE_US);
  uint32_t sumOn = 0;
  for (uint8_t i = 0; i < SAMPLES_PER_PHASE; ++i) sumOn += readADC();

  digitalWrite(IR_LED_PIN, LOW);
  delayMicroseconds(LED_SETTLE_US);
  uint32_t sumOff = 0;
  for (uint8_t i = 0; i < SAMPLES_PER_PHASE; ++i) sumOff += readADC();

  int32_t avgOn  = (int32_t)(sumOn  / SAMPLES_PER_PHASE);
  int32_t avgOff = (int32_t)(sumOff / SAMPLES_PER_PHASE);
  return (int16_t)(avgOn - avgOff);
}

// Compute symmetric thresholds around baseline using a relative margin
static void computeThresholds() {
  float margin = MIN_RELATIVE_DELTA * fabsf(baseline);

  threshUp = baseline + margin;
  threshDn = baseline - margin;

  // diff range is approx [-((2^ADC_BITS)-1), +((2^ADC_BITS)-1)]
  const float DIFF_MAX = (float)((1 << ADC_BITS) - 1);
  const float DIFF_MIN = -DIFF_MAX;

  if (threshUp > DIFF_MAX) threshUp = DIFF_MAX;
  if (threshUp < DIFF_MIN) threshUp = DIFF_MIN;
  if (threshDn > DIFF_MAX) threshDn = DIFF_MAX;
  if (threshDn < DIFF_MIN) threshDn = DIFF_MIN;
}


static void autoCalibrate() {
  detected = false;
  hitCount = 0;
  clearCount = 0;
  digitalWrite(STATUS_LED_PIN, LOW);
  const uint32_t tStart = millis();
  uint32_t n = 0;
  double mean = 0.0, M2 = 0.0;

  while ((millis() - tStart) < CAL_TIME_MS) {
    int16_t d = readDifferenced();
    n++;
    double x = (double)d;
    double delta = x - mean;
    mean += delta / (double)n;
    double delta2 = x - mean;
    M2 += delta * delta2;
    //delay(LOOP_PERIOD_MS);
  }

  if (n > 1) {
    baseline = (float)mean;
    noiseStd = (float)sqrt(M2 / (double)(n - 1));
  } else {
    baseline = 0.0f;
    noiseStd = 5.0f;
  }

  emaDiff = baseline;
  computeThresholds();

  Serial.println(F("\n== Detect Auto-Calibration Complete =="));
  Serial.print(F("Samples: ")); Serial.println((uint32_t)n);
  Serial.print(F("Baseline (diff): ")); Serial.println(baseline, 2);
  Serial.print(F("Noise stdev: "));      Serial.println(noiseStd, 2);
  Serial.print(F("ThreshUp: "));         Serial.println(threshUp);
  Serial.print(F("ThreshDn: "));         Serial.println(threshDn);
  Serial.println(F("================================\n"));
}

// -------------------- PUBLIC API --------------------

// Call once from setup()
void DetectInit(void) {
  pinMode(IR_LED_PIN, OUTPUT);
  digitalWrite(IR_LED_PIN, LOW);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  analogReadResolution(ADC_BITS);
  analogSetAttenuation(ADC_ATTEN);

  autoCalibrate();
}

bool DetectStep(void) {
  int16_t diff = readDifferenced();
  emaDiff = EMA_ALPHA * diff + (1.0f - EMA_ALPHA) * emaDiff;

  float enterLow  = threshDn;
  float enterHigh = threshUp;
  float exitLow   = threshDn + (float)HYSTERESIS_COUNTS;
  float exitHigh  = threshUp - (float)HYSTERESIS_COUNTS;

  // If hysteresis collapses the band, disable hysteresis for clearing
  if (exitLow >= exitHigh) {
    exitLow = enterLow;
    exitHigh = enterHigh;
  }

  if (!detected) {
    // Detect when diff goes outside band (either direction)
    if ((float)diff <= enterLow || (float)diff >= enterHigh) {
      if (++hitCount >= DEBOUNCE_HITS_REQUIRED) {
        detected = true;
        hitCount = 0;
        clearCount = 0;
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.println(F("DETECTED (outside band)"));
      }
    } else {
      hitCount = 0;
    }
  } else {
    // Clear when diff returns inside band (with hysteresis)
    if ((float)diff > exitLow && (float)diff < exitHigh) {
      if (++clearCount >= DEBOUNCE_CLEAR_REQUIRED) {
        detected = false;
        clearCount = 0;
        hitCount = 0;
        digitalWrite(STATUS_LED_PIN, LOW);
        Serial.println(F("CLEARED (inside band)"));
      }
    } else {
      clearCount = 0;
    }
  }

  return detected;
}


bool ActuateSyringeCheckForBubbles(void) {
  autoCalibrate();

  const uint32_t extendMs  = 5000;
  const uint32_t retractMs = 10000;
  const uint32_t postMs    = 2000;   // 1–2 seconds after retract (set to 1000 or 2000)

  bool anyDetected = false;

  setEnable();

  // 1) Extend WITHOUT detection (first step)
  setExtend();
  delay(extendMs);

  // 2) Retract WITH detection (second step)
  setRetract();
  uint32_t tStart = millis();
  while ((millis() - tStart) < retractMs) {
    if (DetectStep()) anyDetected = true;
    delay(LOOP_PERIOD_MS);
  }

  // 3) Disable motor and keep detecting for postMs
  setDisable();
  uint32_t tPost = millis();
  while ((millis() - tPost) < postMs) {
    if (DetectStep()) anyDetected = true;
    delay(LOOP_PERIOD_MS);
  }

  return anyDetected;
}
