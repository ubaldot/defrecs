#include <Arduino.h>

void GPIO_init() { pinMode(LED_BUILTIN, OUTPUT); }

void SerialPort_init() {
  const uint16_t BAUDRATE = 9600;
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;
  };
}
