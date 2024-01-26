//===-------------------- blink_main.c ------------------------*- C -*-===//
//  Each component has a prefix, to be easily searched.
//
//  1. Output must start with the prefix, e.g. blink_led_state
//  2. Mutex for setting/getting must contain the same name of the associated
//     output, e.g. blink_led_state -> mutex_blink_led_state
//  3. Setters and getters shape is the same for all the components and must
//     have the form seto_<output_name>, e.g. seto_blink_seto_state()
//  4. The function to be placed in the scheduling must have suffix _main
//  5. Outputs shall be initialized in the <prefix>_init() function.
//===----------------------------------------------------------------------===//
// blink led component.
//
// prefix: blink_
//
// OUTPUTS: blink_led_state
//===----------------------------------------------------------------------===//

#include "pinout.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static uint8_t blink_led_state;
static SemaphoreHandle_t mutex_blink_led_state;

// Set
static void seto_blink_led_state(const uint8_t *pLedState) {
  if (xSemaphoreTake(mutex_blink_led_state, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&blink_led_state, pLedState, 1);
    xSemaphoreGive(mutex_blink_led_state);
  }
}

// Get
void geto_blink_led_state(uint8_t *pLedState) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_blink_led_state, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(pLedState, &blink_led_state, 1);
    xSemaphoreGive(mutex_blink_led_state);
  }
}

// Init
void blink_init(void) {
  blink_led_state = 0;
  mutex_blink_led_state = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void blink_main() {
  // send something every MAX_COUNT*TASK_PERIOD seconds.
  static uint8_t led_state = 1;
  if (led_state == 0) {
    led_state = 1;
  } else {
    led_state = 0;
  }

  // OUTPUT
  seto_blink_led_state(&led_state);
  pinout_builtin_led(led_state);
}
