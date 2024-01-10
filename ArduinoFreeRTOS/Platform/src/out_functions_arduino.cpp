#include "blink_main.h"
#include "hw_config_arduino.h"
#include "serial_port.h"
#include <Arduino.h>

void out_builtin_led() {
  uint8_t led_state;
  led_state = get_blink_led_state();
  digitalWrite(LED_BUILTIN, led_state);
}

void out_fan1(void) {
  uint8_t const PWM = 3;
  // TODO
  /* pwm = get_something(); */
  analogWrite(FAN1, PWM);
}

void out_serial_port(void) {
  char *msg = serial_get_data();
  /* msg = serial_get_data(); */
  /* Serial.println(msg); */
  /* for (size_t ii = 0; msg[ii] != '\0'; ii++) { */
  /*   Serial.print(msg[ii]); */
  /* } */
  /* Serial.print("\n"); */
}
