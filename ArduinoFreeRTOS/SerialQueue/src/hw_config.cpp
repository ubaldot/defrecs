#include <Arduino.h>

void hw_digital_pins_init() { pinMode(LED_BUILTIN, OUTPUT); }

void hw_pwm_init() {
  const uint8_t FAN1 = 5; // pin 5 is connected to fan1
                          }

  void hw_serial_port_init() {
    const uint16_t BAUDRATE = 9600;
    Serial.begin(BAUDRATE);
    while (!Serial) {
      ;
    };
  }
