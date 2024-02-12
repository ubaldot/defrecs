//===-------------------- serial_port.c ------------------------*- C -*-===//
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
// PREFIX: uart_
// PUBLISHED SIGNALS: change all the time!
//===----------------------------------------------------------------------===//
#include "serial_port.h"
#include "application_setup.h"
#include "blink/blink.h"
#include "interrupts_to_tasks.h"
#include "photovoltaic/pv.h"
#include "pinin.h"
#include "pinout.h"
#include "temperature_sensor/tempsens_LM35.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

uint8_t tx_message[MSG_LENGTH_MAX];

static SemaphoreHandle_t mutex_tx_message;
static SemaphoreHandle_t mutex_rx_message;

void serial_port_init() {
  mutex_tx_message = xSemaphoreCreateMutex();
  mutex_rx_message = xSemaphoreCreateMutex();
}

void serial_port_step(enum WhoIsCalling caller) {
  // INPUTS
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);
  uint8_t msg[MSG_LENGTH_MAX];

  switch (caller) {
  case PERIODIC_TASK:
    strcpy((char *)msg, "Ciao amore.\r\n");
    break;
    /* What starts with IRQ are callbacks! */
  case IRQ_BUILTIN_BUTTON:
    subscribe_irq_builtin_button_message(msg);
    break;
  case IRQ_SERIAL_RX:
    subscribe_irq_raw_rx_message(msg);
    break;
  default:
    strcpy((char *)msg, "Sto cazzo.\r\n");
    break;
  }

  /* float pv_voltage; */
  /* subscribe_pv_voltage(&pv_voltage); */

  /* float tempsens_value; */
  /* subscribe_tempsens_value(&tempsens_value); */

  // Assemble tx_message to be sent
  /* (void)snprintf(tx_message, MSG_LENGTH_MAX, "led state: %d\r\n",
   * led_state); */

  // Cast float readings into string. TODO OBS! dtostrf applies only to
  // Arduino,
  /* const uint8_t MIN_WIDTH = 5; */
  /* char pv_voltage_string[MIN_WIDTH]; */
  /* dtostrf(pv_voltage, MIN_WIDTH, 2, pv_voltage_string); */
  /* char pv_readings[MSG_LENGTH_MAX]; */
  /* (void)snprintf(pv_readings, MSG_LENGTH_MAX, "Photovoltaic reading: %s
   * V",
   */
  /*                pv_voltage_string); */
  /* } */

  if (xSemaphoreTake(mutex_tx_message, pdMS_TO_TICKS(5)) == pdTRUE) {
    memcpy(tx_message, msg, MSG_LENGTH_MAX - 1);
    // To compute the message length we iterate through the array until we
    // encounter the null terminator
    size_t len = 0;
    while (tx_message[len] != '\0') {
      len++;
    }
    pinout_serial_port(tx_message, len);
    xSemaphoreGive(mutex_tx_message);
  }

  // Temperature
  /* char tempsens_value_string[MIN_WIDTH]; */
  /* dtostrf(tempsens_value, MIN_WIDTH, 2, tempsens_value_string); */
  /* char tempsens_readings[MSG_LENGTH_MAX]; */
  /* (void)snprintf(tempsens_readings, MSG_LENGTH_MAX, */
  /*                "Temperature: %s \xC2\xB0 Celsius", tempsens_value_string);
   */
  /* pinout_serial_port(tempsens_readings); */

  // Send the tx_message
  // event-base send tx_message
  // Flipped initialization 0->1 or 1->0, otherwise the led won't trigger.
  /* static uint8_t prev_led_state = (led_state == 1) ? 0 : 1; */
  /* if (prev_led_state != led_state) { */
  /*   pinout_serial_port(tx_message); */
  /*   prev_led_state = led_state; */
  /* } */
}
