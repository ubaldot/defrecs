//===-------------------- tempsens_LM35_main.cpp --------------*- C++ //-*-===//
// Photovoltaic panel reading component.
//  OBS! This depends on LM35 specific sensor!
//
// However, the interface is the same.
//
// prefix: tempsens_
//
// OUTPUTS: tempsens_value
//===----------------------------------------------------------------------===//


#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include "pinin/pinin.h"

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static float tempsens_value;
static SemaphoreHandle_t mutex_tempsens_value;

// Set
static void seto_tempsens_value(const float *pTempValue) {
  if (xSemaphoreTake(mutex_tempsens_value, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&tempsens_value, pTempValue, sizeof(*pTempValue));
    xSemaphoreGive(mutex_tempsens_value);
  }
}

// Get
void geto_tempsens_value(float *pTempValue) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_tempsens_value, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(pTempValue, &tempsens_value, sizeof(*pTempValue));
    xSemaphoreGive(mutex_tempsens_value);
  }
}

// Init
void tempsens_init(void) {
  tempsens_value = 0.0;
  mutex_tempsens_value = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void tempsens_main() {
  // Get a copy of the output
  /* float voltage_reading; */
  /* geto_tempsens_value(&voltage_reading); */

  // Read raw data
  uint16_t analog_value;
  analog_value = pinin_pv();

  float voltage;
  const uint8_t ANALOG_IN_RESOLUTION = 10;
  voltage = ANALOG_IN_RESOLUTION * analog_value;

  // V -> Celsius, [0 5] -> [0 50] linearly.
  float temperature_measured;
  temperature_measured = voltage / (float)10.0;

  // OUTPUT
  seto_tempsens_value(&temperature_measured);
}
