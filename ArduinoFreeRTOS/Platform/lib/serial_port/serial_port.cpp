
#include "debug_main.h"
#include "serial_port.h"
#include "blink_main.h"
#include "pinout_functions_arduino.h"
#include <Arduino.h>

void serial_port_init() {}

void serial_port_main() {
  // INPUTS
  uint8_t led_state;
  get_blink_led_state(&led_state);

  size_t task1000ms_watermark;
  get_debug_task_1000ms_watermark(&task1000ms_watermark);

  size_t task200ms_watermark;
  /* get_debug_task_200ms_watermark(&task200ms_watermark); */

  // Assemble message to be sent
  char message[MESSAGE_SIZE_MAX] = "message";
  /* (void)snprintf( */
  /*     message, MESSAGE_SIZE_MAX, */
  /*     "led state: %d\n task 1000ms watermark: %d\n task 200ms watermark:
   * %d\n", */
  /*     led_state, task1000ms_watermark, task200ms_watermark); */

  // Send the message
  // event-base send message
  // Flipped initialization 0->1 or 1->0, otherwise the led won't trigger.
  static uint8_t prev_led_state = (led_state == 1) ? 0 : 1;
  if (prev_led_state != led_state) {
    pinout_serial_port(message);
    prev_led_state = led_state;
  }
}
