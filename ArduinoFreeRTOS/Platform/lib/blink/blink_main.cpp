#include "serial_port.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// OUTPUTS
static uint8_t blink_led_state;
static SemaphoreHandle_t mutex_blink_led_state;

// Init
void blink_main_init(void) {
  // Mutex for setters/getters
  mutex_blink_led_state = xSemaphoreCreateMutex();
  blink_led_state = 0;
}

// Set
static void set_blink_led_state(uint8_t val) {
  bool isSemaphoreTaken;
  isSemaphoreTaken =
      xSemaphoreTake(mutex_blink_led_state, 100 / portTICK_PERIOD_MS) == pdTRUE;
  if (isSemaphoreTaken) {
    blink_led_state = val;
    xSemaphoreGive(mutex_blink_led_state);
  }
}

// Get
uint8_t get_blink_led_state() {
  uint8_t led_state = 0;
  if (xSemaphoreTake(mutex_blink_led_state, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    led_state = blink_led_state;
    xSemaphoreGive(mutex_blink_led_state);
  }
  return led_state;
}

// ------- Actual function starts here! -------------
void blink_main() {
  // send something every MAX_COUNT*TASK_PERIOD seconds.

  static uint8_t led_state = 0;
  if (led_state == 0) {
    led_state = 1;
  } else {
    led_state = 0;
  }

  set_blink_led_state(led_state);
}
