#include "FreeRTOS.h"
#include "pinout.h"
#include "main.h"
#include "portmacro.h"
#include "string.h"
#include "usart.h"
#include <stddef.h>

void pinout_builtin_led(uint8_t led_state) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, led_state);
}

void pinout_fan1(uint8_t PWM) {}

void pinout_serial_port(const uint8_t *pMessage, size_t len) {

  HAL_UART_Transmit(&huart2, pMessage, len, portMAX_DELAY);
  // Wrapping the String Something with the F() wrapper will move the Strings to
  // Flash memory only rather than to use SRAM space. E.g. Serial.print(F("Sto
  // cazzo"))
}
