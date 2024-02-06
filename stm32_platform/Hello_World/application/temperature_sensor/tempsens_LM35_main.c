//===-------------------- tempsens_LM35_step.cpp --------------*- C++ //-*-===//
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
#include "pinin.h"

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static float tempsens_value;
static SemaphoreHandle_t mutex_tempsens_value;

// Set
static void publish_tempsens_value(const float *pTempValue) {
  if (xSemaphoreTake(mutex_tempsens_value, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&tempsens_value, pTempValue, sizeof(*pTempValue));
    xSemaphoreGive(mutex_tempsens_value);
  }
}

// Get
void subscribe_tempsens_value(float *pTempValue) {
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
void tempsens_step() {
  // Get a copy of the output
  /* float voltage_reading; */
  /* subscribe_tempsens_value(&voltage_reading); */

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
  publish_tempsens_value(&temperature_measured);
}
