//===-------------------- pinin.c --------------*- C //-*-===//
// This is the platform interface between different HAL and the rest of the
// SW.
//
// This is NOT really a component in the sense that does not publish or
// subscribe any signal. For some obscure reasons to myself I decided to not
// consider this as a component.
//
// The functions herein defined can be called by periodically scheduled
// components or by ISR callback functions.
//
//
//===----------------------------------------------------------------------===//

#include "FreeRTOS.h"
#include "adc.h"
#include "application_setup.h"
#include "semphr.h"
#include <string.h>

// Signals
static float adc1_pv_pin_voltage;
static SemaphoreHandle_t mutex_adc1_pv_pin_voltage;

// ADC End Of Conversion (EOC)
extern SemaphoreHandle_t xSemaphoreADC_EOC;
uint8_t ADC1_RESOLUTION_BITS;
const uint8_t NUM_CHANNELS = 2;
float ADC1_PINS_VOLTAGE;
float ADC1_RESOLUTION;

// Publish
static void publish_adc1_pv_pin_voltage(const float *pVoltage) {
  if (xSemaphoreTake(mutex_adc1_pv_pin_voltage, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&adc1_pv_pin_voltage, pVoltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_adc1_pv_pin_voltage);
  }
}

// subscribe
void subscribe_adc1_pv_pin_voltage(float *pVoltage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_adc1_pv_pin_voltage, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(pVoltage, &adc1_pv_pin_voltage, sizeof(*pVoltage));
    xSemaphoreGive(mutex_adc1_pv_pin_voltage);
  }
}

void adc1_sensors_init() {
  switch (hadc1.Init.Resolution) {
  case ADC_RESOLUTION_6B:
    ADC1_RESOLUTION_BITS = 6;
    break;
  case ADC_RESOLUTION_8B:
    ADC1_RESOLUTION_BITS = 8;
    break;
  case ADC_RESOLUTION_10B:
    ADC1_RESOLUTION_BITS = 10;
    break;
  case ADC_RESOLUTION_12B:
    ADC1_RESOLUTION_BITS = 12;
    break;
  }

  /* NUM_CHANNELS = hadc1.Init.NbrOfConversion; */
  ADC1_PINS_VOLTAGE = 3300.0F; // [mV]
  ADC1_RESOLUTION =
      ADC1_PINS_VOLTAGE / (float)(1 << ADC1_RESOLUTION_BITS); // [mV]

  // Dummy init value, useful for debugging
  adc1_pv_pin_voltage = 99.9F;
  // A bit weird to create it here but there is no better place.
  xSemaphoreADC_EOC = xSemaphoreCreateBinary();
  mutex_adc1_pv_pin_voltage = xSemaphoreCreateMutex();
}

void adc1_sensors_step(enum WhoIsCalling caller) {
  (void)caller;
  /* Return the pin voltage in the interval [0, PIN_VOLTAGE] [V]*/
  uint32_t analog_read[NUM_CHANNELS];

  HAL_ADC_Start_DMA(&hadc1, analog_read, NUM_CHANNELS);
  xSemaphoreTake(xSemaphoreADC_EOC, portMAX_DELAY); // Wait for ADC
                                                    // conversion end
  HAL_ADC_Stop_DMA(&hadc1);

  // Process and publish
  float pv_pin_voltage;
  pv_pin_voltage = ADC1_RESOLUTION * (float)analog_read[0] / 1000.0F;
  publish_adc1_pv_pin_voltage(&pv_pin_voltage);
}
