#include "blink_main.h"
#include <Arduino.h>

void out_builtin_led() {
  uint8_t led_state;
  led_state = get_led_state();
  digitalWrite(LED_BUILTIN, led_state);
}
