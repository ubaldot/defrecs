//===-------------------- serial_port.c ------------------------*- C -*-===//
// Handle the serial port.
//
// PREFIX: serial_port_
// PUBLISHED SIGNALS: None.
//===----------------------------------------------------------------------===//
#include "serial_port.h"
#include "application_setup.h"
#include "blink/blink.h"
#include "hmi/hmi.h"
#include "usart.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <semphr.h>
#include <stddef.h>

static char rx_char;

static SemaphoreHandle_t mutex_tx_process;
static SemaphoreHandle_t mutex_rx_char;

// Publish
static void publish_serial_port_rx_msg(const uint8_t *pMsg) {
  if (xSemaphoreTake(mutex_rx_char, 100 / portTICK_PERIOD_MS) == pdTRUE) {

    memcpy(&rx_char, pMsg, 1);
    xSemaphoreGive(mutex_rx_char);
  }
}

void subscribe_serial_port_rx_msg(char *pMsg) {
  if (xSemaphoreTake(mutex_rx_char, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(pMsg, &rx_char, 1);
    xSemaphoreGive(mutex_rx_char);
  }
}

void serial_port_init(void) {
  mutex_tx_process = xSemaphoreCreateMutex();
  mutex_rx_char = xSemaphoreCreateMutex();
  uint8_t rx_buffer = '\0';
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_buffer, 1);
}

void serial_port_write_step(enum WhoIsCalling caller) {
  (void)caller;
  if (xSemaphoreTake(mutex_tx_process, pdMS_TO_TICKS(5)) == pdTRUE) {
    uint8_t tx_buffer[MSG_LENGTH_MAX];
    subscribe_hmi_tx_msg(tx_buffer);
    HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, strlen(tx_buffer),
        portMAX_DELAY);
    xSemaphoreGive(mutex_tx_process);
  }
}

void serial_port_read_step(enum WhoIsCalling caller) {
  (void)caller;
  uint8_t tmp;
  HAL_UART_Receive_IT(&huart2, (uint8_t *)tmp, 1);
  publish_serial_port_rx_msg(&tmp);
}
