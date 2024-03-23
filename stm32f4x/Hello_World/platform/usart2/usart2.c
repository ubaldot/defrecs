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
// PUBLISHED SIGNALS: None.
//===----------------------------------------------------------------------===//
#include "usart2/usart2.h"
#include "application_setup.h"
#include "blink/blink.h"
#include "ftoa.h"
#include "interrupts_to_tasks.h"
#include "usart.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

static void transmit(char *pMsg);

/* Semaphore to prevent race conditions in HAL_UART_Transmit */
/* Note that this function does not publish any signal, hence no other mutexes */
/* are needed */
static SemaphoreHandle_t mutex_tx_process;

/* Init */
void usart2_init() { mutex_tx_process = xSemaphoreCreateMutex(); }

/* Step */
void usart2_step(enum WhoIsCalling caller) {
  // INPUTS
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);

  char msg[MSG_LENGTH_MAX];

  switch (caller) {
  case PERIODIC_TASK:

    (void)snprintf(msg, MSG_LENGTH_MAX, "Led state: %d \n", led_state);
    transmit(msg);
    break;
    /* What starts with IRQ are callbacks! */
  case IRQ_BUILTIN_BUTTON:
    strncpy(msg, "Hello World!\n", MSG_LENGTH_MAX);
    transmit(msg);
    break;

  default:
    strncpy(msg, "FooFoo.\n", MSG_LENGTH_MAX - 1);
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
