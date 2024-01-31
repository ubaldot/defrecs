//===-------------------- debug_main.c ------------------------*- C -*-===//
// Used for debugging.
//
// prefix: debug_
//
//===----------------------------------------------------------------------===//

#include "FreeRTOS.h"
#include <semphr.h>
#include <string.h>

extern TaskHandle_t xTaskHandle_200ms;
extern TaskHandle_t xTaskHandle_1000ms;

// Init
void debug_init(void) {}

// ------- Actual function starts here! -------------
void debug_main() {

  volatile size_t task_200ms_watermark;
  task_200ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_200ms);
  volatile size_t task_1000ms_watermark;
  task_1000ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_1000ms);
}
