//===-- hw_config_arduino.h - HW config for Arduino boards-------*- C++ -*-===//
// This is used for initializing Arduino boards hardware.
// Unfortunately, some config require some function call and this is the
// reason why we use hw_config_arduino_init().
//===----------------------------------------------------------------------===//

#ifndef HW_CONFIG_ARDUINO_H
#define HW_CONFIG_ARDUINO_H

#include <Arduino.h>

// Constants
// Analog out
const uint8_t FAN1 = 5; // pin 5 is connected to fan1

// Init contains stuff that must be initialized by function call.
void hw_config_arduino_init(void);

#endif /* HW_CONFIG_ARDUINO_H */
