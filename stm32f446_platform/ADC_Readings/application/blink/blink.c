//===-------------------- blink.c ------------------------*- C -*-===//
// Simply toggle the led state.
//
// PREFIX: blink_
// PUBLISHED SIGNALS: blink_led_state
//===----------------------------------------------------------------------===//

/* #include "application_setup.h" */
#include "application_setup.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static uint8_t blink_led_state;
static SemaphoreHandle_t mutex_blink_led_state;

// Publish
static void publish_blink_led_state(const uint8_t *pLedState) {
  if (xSemaphoreTake(mutex_blink_led_state, pdMS_TO_TICKS(5)) == pdTRUE) {
    memcpy(&blink_led_state, pLedState, 1);
    xSemaphoreGive(mutex_blink_led_state);
  }
}

// Subscribe (for the others)
void subscribe_blink_led_state(uint8_t *pLedState) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_blink_led_state, pdMS_TO_TICKS(5)) == pdTRUE) {
    memcpy(pLedState, &blink_led_state, 1);
    xSemaphoreGive(mutex_blink_led_state);
  }
}

// Init
void blink_init(void) {
  blink_led_state = 0;
  mutex_blink_led_state = xSemaphoreCreateMutex();
}

// Step
void blink_step(enum WhoIsCalling caller) {
  (void)caller;
  /* We use a local led_state to avoid race condition. */
  /* In this way each task calling blink_step will have its own */
  /* local copy of led_state in the stack, but the publishing operation */
  /* is protected by a mutex. */
  /* Perhaps a more robust solution is to assign led_state through
   * subscribe_blink_led_state though. */
  uint8_t led_state;
  led_state = blink_led_state ? 0 : 1;

  // OUTPUT
  publish_blink_led_state(&led_state);
}
