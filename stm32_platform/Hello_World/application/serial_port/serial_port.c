#include "serial_port.h"
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

#define MESSAGE_SIZE_MAX 100
char tx_message[MESSAGE_SIZE_MAX];
char rx_message[MESSAGE_SIZE_MAX];

static SemaphoreHandle_t mutex_serial_pinout;

void serial_port_init() { mutex_serial_pinout = xSemaphoreCreateMutex(); }

void serial_port_main(void *pIRQparams) {
  // INPUTS
  uint8_t led_state;
  geto_blink_led_state(&led_state);

  if (pIRQparams != NULL) {
    const char *msg = (const char *)pIRQparams;
    // TODO:
    /* if (TX == 1) { */
    strncpy(tx_message, msg, MESSAGE_SIZE_MAX - 1);
    /*   tx_message[MESSAGE_SIZE_MAX - 1] = '\0'; */
    /* } else { */
    /*   pinin_usart(rx_message); */
    /* } */

    /* } else { */
    /* float pv_voltage; */
    /* geto_pv_voltage(&pv_voltage); */

    /* float tempsens_value; */
    /* geto_tempsens_value(&tempsens_value); */

    // Assemble tx_message to be sent
    (void)snprintf(tx_message, MESSAGE_SIZE_MAX, "led state: %d\r\n",
                   led_state);

    // Cast float readings into string. TODO OBS! dtostrf applies only to
    // Arduino,
    /* const uint8_t MIN_WIDTH = 5; */
    /* char pv_voltage_string[MIN_WIDTH]; */
    /* dtostrf(pv_voltage, MIN_WIDTH, 2, pv_voltage_string); */
    /* char pv_readings[MESSAGE_SIZE_MAX]; */
    /* (void)snprintf(pv_readings, MESSAGE_SIZE_MAX, "Photovoltaic reading: %s
     * V",
     */
    /*                pv_voltage_string); */
  }

  if (xSemaphoreTake(mutex_serial_pinout, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    /* TaskHandle_t current_task; */
    /* current_task = xTaskGetCurrentTaskHandle(); */
    /* BaseType_t current_task_prio = xTaskPriorityGet(current_task); */
    /* xTaskPrioritySet(current_task, configMAX_PRIORITIES); */

    /* xTaskPriorityInherit(); */
    /* Shared resource */
    pinout_serial_port(tx_message);

    xSemaphoreGive(mutex_serial_pinout);
    /* xTaskPrioritySet(current_task, current_task_prio); */
  }

  // Temperature
  /* char tempsens_value_string[MIN_WIDTH]; */
  /* dtostrf(tempsens_value, MIN_WIDTH, 2, tempsens_value_string); */
  /* char tempsens_readings[MESSAGE_SIZE_MAX]; */
  /* (void)snprintf(tempsens_readings, MESSAGE_SIZE_MAX, */
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
