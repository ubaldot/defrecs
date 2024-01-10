
#include "serial_port.h"
#include "../blink/blink_main.h"
#include "pinout_functions_arduino.h"

void serial_port_init() {}

void serial_port_main() {
  // INPUTS
  uint8_t led_state;
  get_blink_led_state(&led_state);

  // Assemble message to be sent
  char message[MESSAGE_SIZE_MAX];
  (void)snprintf(message, MESSAGE_SIZE_MAX, "led state: %d", led_state);

  // Send the message
  // event-base send message
  // Flipped initialization 0->1 or 1->0, otherwise the led won't trigger.
  static uint8_t prev_led_state = (led_state == 1) ? 0 : 1;
  if (prev_led_state != led_state) {
    pinout_serial_port(message);
    prev_led_state = led_state;
  }
}
