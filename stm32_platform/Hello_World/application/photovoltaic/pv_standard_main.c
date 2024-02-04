//===-------------------- pv_standard_main.cpp --------------*- C++ //-*-===//
// Photovoltaic panel reading component.
//  OBS! This depends on the voltage meter used! In this case we assume a
//  standard voltage meter such that the max reading is 25V.
//
// However, the interface is the same.
//
// prefix: pv_
//
// OUTPUTS: pv_voltage
//===----------------------------------------------------------------------===//


#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include "pinin.h"
#include "application_setup.h"

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static float pv_voltage;
static SemaphoreHandle_t mutex_pv_voltage;

// Set
static void seto_pv_voltage(const float *pVoltage) {
  if (xSemaphoreTake(mutex_pv_voltage, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(&pv_voltage, pVoltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_pv_voltage);
  }
}

// Get
void geto_pv_voltage(float *pVoltage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_pv_voltage, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(pVoltage, &pv_voltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_pv_voltage);
  }
}

// Init
void pv_init(void) {
  pv_voltage = 0.0;
  mutex_pv_voltage = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void pv_main(enum WhoIsCalling caller) {
    (void)caller;
  float voltage;
  geto_pv_voltage(&voltage);

  // Read raw data
  uint16_t analog_value;
  analog_value = pinin_pv();

  float temp;
  // TODO:
  const uint8_t ANALOG_IN_RESOLUTION = 10;
  temp = ANALOG_IN_RESOLUTION * analog_value; // [mV] in the coordinates [0,5]

  // Map V -> V,[0,5] linearly to [0,25]
  // This voltage meter maps [0,5] linearly to [0,25]
  voltage = temp * (float)5.0/(float)1000.0;

  // OUTPUT
  seto_pv_voltage(&voltage);
}
