
#include "pinin.h"
#include "FreeRTOS.h"
#include "application_setup.h"
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

void pinin_usart(uint8_t *pMessage, enum HAL_Function_Mode mode) {
  HAL_StatusTypeDef status = HAL_OK;
  switch (mode) {
  case POLLING:
    // AT 9600 Baud we have 1 bytes/ms transfer rate. We add 5ms spare.
    status = HAL_UART_Receive(&huart2, pMessage, MSG_LENGTH_MAX,
                              /* pdMS_TO_TICKS(RX_MSG_LENGTH_MAX + 5)); */
                              pdMS_TO_TICKS(5000));
  case INTERRUPT:
    status = HAL_UART_Receive_IT(&huart2, pMessage, 1);
  case DMA:;
  }
  if (status != HAL_OK) {
    // If something goes wrong something
  }
}
