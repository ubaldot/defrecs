#include "pinout_arduino.h"
#include "hw_config_arduino.h"
#include "serial_port.h"
#include <Arduino.h>

void pinout_builtin_led(uint8_t led_state) {
  digitalWrite(LED_BUILTIN, led_state);
}

void pinout_fan1(uint8_t PWM) { analogWrite(FAN1, PWM); }

void pinout_serial_port(const char *pMessage) {
  for (size_t ii = 0; pMessage[ii] != '\0'; ii++) {
    Serial.print(pMessage[ii]);
  }
  // Wrapping the String Something with the F() wrapper will move the Strings to
  // Flash memory only rather than to use SRAM space. E.g. Serial.print(F("Sto
  // cazzo"))
  Serial.print("\n");
}
