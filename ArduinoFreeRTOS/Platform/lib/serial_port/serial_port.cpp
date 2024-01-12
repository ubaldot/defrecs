
#include "serial_port.h"
#include "blink_main.h"
#include "debug_main.h"
#include "pinout_arduino.h"
#include "pv_main.h"
#include "tempsens_main.h"
#include <Arduino.h>

void serial_port_init() {}

void serial_port_main() {
  // INPUTS
  uint8_t led_state;
  geto_blink_led_state(&led_state);

  size_t task1000ms_watermark;
  geto_debug_task_1000ms_watermark(&task1000ms_watermark);

  size_t task200ms_watermark;
  geto_debug_task_200ms_watermark(&task200ms_watermark);

  float pv_voltage;
  geto_pv_voltage(&pv_voltage);

  float tempsens_value;
  geto_tempsens_value(&tempsens_value);

  // Assemble message to be sent
  /* char message[MESSAGE_SIZE_MAX]; */
  /* (void)snprintf( */
  /*     message, MESSAGE_SIZE_MAX, */
  /*     "led state: %d\ntask 1000ms watermark: %d\ntask 200ms watermark: %d\n",
   */
  /*     led_state, task1000ms_watermark, task200ms_watermark); */

  // Cast float readings into string. TODO OBS! dtostrf applies only to Arduino,
  const uint8_t MIN_WIDTH = 5;

  char pv_voltage_string[MIN_WIDTH];
  dtostrf(pv_voltage, MIN_WIDTH, 2, pv_voltage_string);
  char pv_readings[MESSAGE_SIZE_MAX];
  (void)snprintf(pv_readings, MESSAGE_SIZE_MAX, "Photovoltaic reading: %s V",
                 pv_voltage_string);
  pinout_serial_port(pv_readings);

  // Temperature
  /* char tempsens_value_string[MIN_WIDTH]; */
  /* dtostrf(tempsens_value, MIN_WIDTH, 2, tempsens_value_string); */
  /* char tempsens_readings[MESSAGE_SIZE_MAX]; */
  /* (void)snprintf(tempsens_readings, MESSAGE_SIZE_MAX, */
  /*                "Temperature: %s \xC2\xB0 Celsius", tempsens_value_string); */
  /* pinout_serial_port(tempsens_readings); */

  // Send the message
  // event-base send message
  // Flipped initialization 0->1 or 1->0, otherwise the led won't trigger.
  /* static uint8_t prev_led_state = (led_state == 1) ? 0 : 1; */
  /* if (prev_led_state != led_state) { */
  /*   pinout_serial_port(message); */
  /*   prev_led_state = led_state; */
  /* } */
}
