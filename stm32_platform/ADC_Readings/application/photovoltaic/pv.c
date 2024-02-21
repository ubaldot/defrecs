//===-------------------- pv_standard_step.c --------------*- C //-*-===//
// Photovoltaic panel reading component.
//  OBS! This depends on the voltage meter used! In this case we assume a
//  standard voltage meter such that the max reading is 25V.
//
// However, the interface is the same.
//
// PREFIX: pv_
// PUBLISHED SIGNALS: pv_voltage
//===----------------------------------------------------------------------===//

#include "adc1_sensors/adc1_sensors.h"
#include "application_setup.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static float pv_voltage;
static SemaphoreHandle_t mutex_pv_voltage;

// Publish
static void publish_pv_voltage(const float *pVoltage) {
  if (xSemaphoreTake(mutex_pv_voltage, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(&pv_voltage, pVoltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_pv_voltage);
  }
}

// subscribe
void subscribe_pv_voltage(float *pVoltage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_pv_voltage, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(pVoltage, &pv_voltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_pv_voltage);
  }
}

// Init
void pv_init(void) {
  pv_voltage = 0.0F;
  mutex_pv_voltage = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void pv_step(enum WhoIsCalling caller) {
  (void)caller;
  // INPUT
  // Read pin voltage [V]
  float pin_voltage;
  subscribe_adc1_pv_pin_voltage(&pin_voltage);

  // Map V -> V,[0,5] linearly to [0,25]
  // This voltage meter maps [0,5] linearly to [0,25] and convert mV to V.
  float pv_panel_voltage;
  pv_panel_voltage = pin_voltage * 5.0F;

  // OUTPUT
  publish_pv_voltage(&pv_panel_voltage);
}
