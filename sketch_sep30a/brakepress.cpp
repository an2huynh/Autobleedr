#include "brakepress.h"
#include <stdint.h>

static void busy_delay_ms(uint32_t ms) {
  volatile uint32_t t = 0;
  for (uint32_t i = 0; i < ms * 16000u; ++i) { t ^= i; }
}

bool HandBrakePress(void) {
  busy_delay_ms(200);
  return true;
}
