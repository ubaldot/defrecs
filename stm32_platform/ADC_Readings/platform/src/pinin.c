
#include "pinin.h"
#include "FreeRTOS.h"
#include "application_setup.h"
#include "serial_port/serial_port.h"
#include "usart.h"

float pinin_pv(void) {
  /* Return the pin voltage in the interval [0, PIN_VOLTAGE] [V]*/
  extern ADC_HandleTypeDef hadc1;
  const uint32_t ADC_RESOLUTION_BITS = 10;
  // TODO: use some get function
  /* const uint32_t pippo = hadc1.Init.Resolution; // For UNO, in bits */
  const float PIN_VOLTAGE = 5000.0F;            // [mV]
  const float ANALOG_IN_RESOLUTION =
      PIN_VOLTAGE / (float)(1 << ADC_RESOLUTION_BITS); // [mV]

  /* size_t analog_read = readAnalog(PV); */
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  HAL_ADC_Stop(&hadc1);
  size_t analog_read = HAL_ADC_GetValue(&hadc1);
  float pin_voltage;
  pin_voltage = ANALOG_IN_RESOLUTION * (float)analog_read;

  return pin_voltage / 1000.0F;
}

float pinin_pv1(void) { return 1; }

void pinin_usart(uint8_t *pMessage, size_t len, enum HAL_Function_Mode mode) {
  HAL_StatusTypeDef status = HAL_OK;
  switch (mode) {
  case POLLING:
    // AT 9600 Baud we have 1 bytes/ms transfer rate. We add 5ms spare.
    status = HAL_UART_Receive(&huart2, pMessage, len,
                              /* pdMS_TO_TICKS(RX_MSG_LENGTH_MAX + 5)); */
                              pdMS_TO_TICKS(5000));
  case INTERRUPT:
    status = HAL_UART_Receive_IT(&huart2, pMessage, len);
  case DMA:;
  }
  if (status != HAL_OK) {
    // If something goes wrong something
  }
}
