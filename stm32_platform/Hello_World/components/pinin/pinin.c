
#include "pinin.h"

uint16_t pinin_pv(void) {
  // TODO: use some get function
  const uint16_t ADC_RESOLUTION_BITS = 10; // For UNO, in bits
  const float pin_voltage = 5000.0;        // [mV]
  const float ANALOG_IN_RESOLUTION =
      pin_voltage / (float)(1 << ADC_RESOLUTION_BITS); // [mV]

  /* size_t analog_read = readAnalog(PV); */
  float voltage;
  /* voltage = ANALOG_IN_RESOLUTION * analog_read; */

  return voltage;
}

uint16_t pinin_pv1(void) { return 1; }
