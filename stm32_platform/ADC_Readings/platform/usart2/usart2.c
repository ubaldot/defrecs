//===-------------------- usart2.c ------------------------*- C -*-===//
// This component does three things:
//
//  1. Send messages over the UART,
//  2. Process received raw data,
//  3. Publish processed received raw data into signals that can be subscribed
//     by other components.
//
// The received messages are read byte by byte through interrupts. When a
// terminator character is detected, then the task associated to the ISR
// update a published signal.
//
// PREFIX: usart2_
// PUBLISHED SIGNALS: change all the time!
//===----------------------------------------------------------------------===//
#include "usart2/usart2.h"
#include "application_setup.h"
#include "blink/blink.h"
#include "ftoa.h"
#include "interrupts_to_tasks.h"
#include "photovoltaic/pv.h"
#include "temperature_sensor/tempsens_LM35.h"
#include "usart.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

static void transmit(char *pMsg);

static char rx_buffer[MSG_LENGTH_MAX];
static size_t ii; // For counting the number of bytes received

static SemaphoreHandle_t mutex_tx_process;
static SemaphoreHandle_t mutex_rx_buffer; // This also protect ii

void usart2_init() {
  mutex_tx_process = xSemaphoreCreateMutex();
  mutex_rx_buffer = xSemaphoreCreateMutex();

  ii = 0;
  rx_buffer[ii] = '\0';
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_buffer[ii], 1);
}

void usart2_step(enum WhoIsCalling caller) {
  // INPUTS
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);
  float pv_voltage;
  float tempsens_C;

  char msg[MSG_LENGTH_MAX];

  switch (caller) {
  case PERIODIC_TASK:
    subscribe_pv_voltage(&pv_voltage);

    char pv_voltage_str[5];
    (void)ftoa(pv_voltage, pv_voltage_str, 2);
    (void)snprintf(msg, MSG_LENGTH_MAX, "Photovoltaic reading: %s V\n",
                   pv_voltage_str);

    transmit(msg);
    break;
    /* What starts with IRQ are callbacks! */
  case IRQ_BUILTIN_BUTTON:
    subscribe_tempsens_value(&tempsens_C);

    char tempsens_C_str[5];
    (void)ftoa(tempsens_C, tempsens_C_str, 2);
    (void)snprintf(msg, MSG_LENGTH_MAX, "Temperature: %s C \n", tempsens_C_str);

    transmit(msg);
    break;
  case IRQ_SERIAL_RX:
    if (xSemaphoreTake(mutex_rx_buffer, pdMS_TO_TICKS(5)) == pdTRUE) {
      if (rx_buffer[ii] == '\n' || ii > MSG_LENGTH_MAX) {
        /* publish_usart2_rx_message(msg, strlen(msg)); */
        memcpy(msg, rx_buffer, MSG_LENGTH_MAX - 1);
        msg[MSG_LENGTH_MAX - 1] = '\0';
        transmit(msg);

        /* Reinitialize all the variables used */
        ii = 0;
        memset(rx_buffer, '\0', MSG_LENGTH_MAX);
      } else {
        ii++;
      }
      HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_buffer[ii], 1);
      xSemaphoreGive(mutex_rx_buffer);
    }
    break;
  default:
    strncpy(msg, "Sto cazzo.\n", MSG_LENGTH_MAX - 1);
    msg[MSG_LENGTH_MAX - 1] = '\0';
    break;
  }
}

void transmit(char *pMsg) {
  if (xSemaphoreTake(mutex_tx_process, pdMS_TO_TICKS(5)) == pdTRUE) {
    HAL_UART_Transmit(&huart2, (uint8_t *)pMsg, strlen(pMsg), portMAX_DELAY);
    xSemaphoreGive(mutex_tx_process);
  }
}
