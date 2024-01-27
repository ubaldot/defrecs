//===-- hw_config_arduino.h - HW config for Arduino boards-------*- C++ -*-===//
//
// This is used for initializing Arduino boards hardware.
// Unfortunately, some config require some function call and this is the
// reason why we use hw_config_arduino_init().
//
//===----------------------------------------------------------------------===//

#ifndef HW_CONFIG_ARDUINO_H
#define HW_CONFIG_ARDUINO_H

#include <Arduino.h>

// Analog in
// const uint16_t ADC_RESOLUTION = analogReadResolution(whatever_bits); Only
// valid for Zero, Due, MKR Family, etc.
const uint16_t ADC_RESOLUTION = 10;                // For UNO, in bits
const float ANALOG_IN_RESOLUTION = 5000.0 / 1024.0; // [mV]
const uint16_t PV = A5;
const uint16_t PV2 = A4;

// Temperature sensor
const uint16_t TEMP_SENS = A1;

// Analog out
const uint8_t FAN1 = 5; // pin 5 is connected to fan1

//
// Init contains stuff that must be initialized by function call.
void hw_config_arduino_init(void);

#endif /* HW_CONFIG_ARDUINO_H */
