
#include "serial_port.h"
#include "../blink/blink_main.h"
#include "pinout_functions_arduino.h"

// This function is the main. If there is anything in the queue it just send
// it to the serial port.
void serial_port_main() {
  // INPUTS
  uint8_t led_state;
  get_blink_led_state(&led_state);

  // Assemble message to be sent
  char message[MESSAGE_SIZE_MAX];
  (void)snprintf(message, MESSAGE_SIZE_MAX, "led state: %d", led_state);

  // Send the message
  pinout_serial_port(message);
}
