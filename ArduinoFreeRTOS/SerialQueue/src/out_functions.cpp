#include "blink_main.h"
#include "hw_config.h"
#include <Arduino.h>

void out_builtin_led() {
  uint8_t led_state;
  led_state = get_blink_led_state();
  digitalWrite(LED_BUILTIN, led_state);
}

/* void out_fan1(pwm) { analogWrite(FAN1, pwm); } */
