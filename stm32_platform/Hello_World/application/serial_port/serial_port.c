#include "serial_port.h"
#include "blink/blink_main.h"
#include "photovoltaic/pv_main.h"
#include "pinout.h"
#include "temperature_sensor/tempsens_main.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MESSAGE_SIZE_MAX 100
char tx_message[MESSAGE_SIZE_MAX];
char rx_message[MESSAGE_SIZE_MAX];

void serial_port_init() {}

void serial_port_main(void *pIRQparams) {
  // INPUTS
  uint8_t led_state;
  geto_blink_led_state(&led_state);

  if (pIRQparams != NULL) {
    const char *msg = (const char *)pIRQparams;
    strncpy(tx_message, msg, MESSAGE_SIZE_MAX - 1);
    tx_message[MESSAGE_SIZE_MAX - 1] = '\0';

  } else {
    /* float pv_voltage; */
    /* geto_pv_voltage(&pv_voltage); */

    /* float tempsens_value; */
    /* geto_tempsens_value(&tempsens_value); */

    // Assemble tx_message to be sent
    (void)snprintf(tx_message, MESSAGE_SIZE_MAX, "led state: %d\r\n", led_state);

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

  pinout_serial_port(tx_message);

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
