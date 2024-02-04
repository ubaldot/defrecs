
#include "pinin.h"
#include "FreeRTOS.h"
#include "serial_port/serial_port.h"
#include "usart.h"

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

void pinin_usart(char *pMessage) {
  HAL_StatusTypeDef status;
  status = HAL_UART_Receive(&huart2, (uint8_t *)pMessage, 4, portMAX_DELAY);
  if (status != HAL_OK) {
    // If something goes wrong something
  }
}
