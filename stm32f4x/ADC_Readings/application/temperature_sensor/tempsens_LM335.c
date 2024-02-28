//===-------------------- tempsens_LM335.c ------------------------*- C -*-===//
// Temperature sensor component. Based on LM335.
//
// PREFIX: tempsens_
// PUBLISHED SIGNALS: tempsens_value
//===----------------------------------------------------------------------===//

#include "adc1_sensors/adc1_sensors.h"
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
void tempsens_step(enum WhoIsCalling caller) {
  (void)caller;
  // Get a copy of the output
  float pin_voltage = 99.9F;
  subscribe_adc1_tempsens_pin_voltage(&pin_voltage);

  // V -> Celsius, [0 5] -> [0 50] linearly.
  float temperature_measured;
  temperature_measured = (pin_voltage - 2.73F) * 100.0F;

  // OUTPUT
  publish_tempsens_value(&temperature_measured);
}
