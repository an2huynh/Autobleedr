#include "detect.h"
#include "syringe.h"

static bool bubbles_present_mock() { return true; }

bool ActuateSyringeCheckForBubbles(void) {
  (void)ActuateSyringe();
  return bubbles_present_mock();
}
