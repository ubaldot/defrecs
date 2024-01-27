
#include <Arduino.h>

void hw_config_arduino_init() {
  // Digital GPIO
  pinMode(LED_BUILTIN, OUTPUT);

  // Serial port
  const uint16_t BAUDRATE = 9600;
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;
  };
}
