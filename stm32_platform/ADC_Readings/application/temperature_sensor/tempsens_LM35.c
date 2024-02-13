//===-------------------- tempsens.c ------------------------*- C -*-===//
// Temperature sensor component.
//
// PREFIX: tempsens_
// PUBLISHED SIGNALS: tempsens_value
//===----------------------------------------------------------------------===//

#include "pinin.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static float tempsens_value;
static SemaphoreHandle_t mutex_tempsens_value;

// Publish
static void publish_tempsens_value(const float *pTempValue) {
  if (xSemaphoreTake(mutex_tempsens_value, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&tempsens_value, pTempValue, sizeof(*pTempValue));
    xSemaphoreGive(mutex_tempsens_value);
  }
}

// Subscribe
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
  tempsens_value = 0.0F;
  mutex_tempsens_value = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void tempsens_step() {
  // Get a copy of the output
  /* float voltage_reading; */
  /* subscribe_tempsens_value(&voltage_reading); */

  // Read pin voltage
  float pin_voltage = 0.0F;
  pinin_pv(&pin_voltage, DMA);

  // V -> Celsius, [0 5] -> [0 50] linearly.
  float temperature_measured;
  temperature_measured = pin_voltage / 10.0F;

  // OUTPUT
  publish_tempsens_value(&temperature_measured);
}
