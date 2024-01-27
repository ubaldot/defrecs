#include "pinin_arduino.h"
#include "hw_config_arduino.h"
#include <Arduino.h>

uint16_t pinin_pv(void) { return analogRead(PV); }

uint16_t pinin_pv1(void) { return analogRead(PV2); }
