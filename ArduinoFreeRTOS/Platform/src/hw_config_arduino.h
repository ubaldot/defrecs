#ifndef HW_CONFIG_ARDUINO_H
#define HW_CONFIG_ARDUINO_H

#include <Arduino.h>

// Constants
// Analog out
const uint8_t FAN1 = 5; // pin 5 is connected to fan1

// Init contains stuff that must be initialized by function call.
void hw_config_arduino_init(void);

#endif /* HW_CONFIG_ARDUINO_H */
