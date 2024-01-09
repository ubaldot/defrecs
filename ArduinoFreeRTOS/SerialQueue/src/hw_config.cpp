#include <Arduino.h>

void hw_digital_pins_config() { pinMode(LED_BUILTIN, OUTPUT); }

void hw_analog_out_pins_config() {
  const uint8_t FAN1 = 5; // pin 5 is connected to fan1
}

void hw_serial_port_config() {
  const uint16_t BAUDRATE = 9600;
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;
  };
}
