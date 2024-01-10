
#include "hw_config_arduino.h"
#include "tasks_setup.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

void setup() {

  hw_config_arduino_init();
  tasks_setup();
}

// Skip loop. OBS! IdleTask cannot be used!
void loop() {}
