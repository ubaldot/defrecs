#include "serial_port.h"
#include "application_setup.h"
#include "blink/blink_main.h"
#include "photovoltaic/pv_main.h"
#include "pinin.h"
#include "pinout.h"
#include "temperature_sensor/tempsens_main.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

char tx_message[TX_MSG_LENGTH_MAX];
char rx_message[RX_MSG_LENGTH_MAX];

static SemaphoreHandle_t mutex_tx_message;
static SemaphoreHandle_t mutex_rx_message;
/* static SemaphoreHandle_t mutex_serial_pinin; */
/* static volatile uint8_t UsartReady; */

void serial_port_init() {
  mutex_tx_message = xSemaphoreCreateMutex();
  mutex_rx_message = xSemaphoreCreateMutex();
  pinin_usart(rx_message, INTERRUPT); // Initialize for reception
}

/* void process_rx_char(char c) { strncpy(msg, rx_message, TX_MSG_LENGTH_MAX -
 * 1); } */

void serial_port_main(enum WhoIsCalling caller) {
  // INPUTS
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);

  char msg[TX_MSG_LENGTH_MAX];
  switch (caller) {
  case PERIODIC_TASK:
    strncpy(msg, "Ciao amore.\r\n", TX_MSG_LENGTH_MAX - 1);
    break;
    /* What starts with IRQ are callbacks! */
  case IRQ_BUILTIN_BUTTON:
    strncpy(msg, "Ciao fata.\r\n", TX_MSG_LENGTH_MAX - 1);
    break;
  case IRQ_SERIAL_RX:
    // Process the received message and place it in the right SO.
    if (xSemaphoreTake(mutex_rx_message, 100 / portTICK_PERIOD_MS) == pdTRUE) {
      pinin_usart(rx_message, POLLING);
      // Re-transmit the received message over the uart
      strncpy(msg, rx_message, RX_MSG_LENGTH_MAX - 1);
      pinin_usart(rx_message, INTERRUPT);
      /* process_rx_char(rx_message); */
      xSemaphoreGive(mutex_rx_message);
    }
    break;
  default:
    strncpy(msg, "Ciao stocazzo.\r\n", TX_MSG_LENGTH_MAX - 1);
    break;
  }

  /* float pv_voltage; */
  /* subscribe_pv_voltage(&pv_voltage); */

  /* float tempsens_value; */
  /* subscribe_tempsens_value(&tempsens_value); */

  // Assemble tx_message to be sent
  /* (void)snprintf(tx_message, TX_MSG_LENGTH_MAX, "led state: %d\r\n",
   * led_state); */

  // Cast float readings into string. TODO OBS! dtostrf applies only to
  // Arduino,
  /* const uint8_t MIN_WIDTH = 5; */
  /* char pv_voltage_string[MIN_WIDTH]; */
  /* dtostrf(pv_voltage, MIN_WIDTH, 2, pv_voltage_string); */
  /* char pv_readings[TX_MSG_LENGTH_MAX]; */
  /* (void)snprintf(pv_readings, TX_MSG_LENGTH_MAX, "Photovoltaic reading: %s
   * V",
   */
  /*                pv_voltage_string); */
  /* } */

  if (xSemaphoreTake(mutex_tx_message, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    strncpy(tx_message, msg, TX_MSG_LENGTH_MAX - 1);
    // TODO: the following may be redundant.
    tx_message[TX_MSG_LENGTH_MAX - 1] = '\0'; // Ensure null-termination
    xSemaphoreGive(mutex_tx_message);
    /* HAL_UART_Transmit is blocking, so we don't need a semaphore*/
    pinout_serial_port(tx_message);
  }

  // Temperature
  /* char tempsens_value_string[MIN_WIDTH]; */
  /* dtostrf(tempsens_value, MIN_WIDTH, 2, tempsens_value_string); */
  /* char tempsens_readings[TX_MSG_LENGTH_MAX]; */
  /* (void)snprintf(tempsens_readings, TX_MSG_LENGTH_MAX, */
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
