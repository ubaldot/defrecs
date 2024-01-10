//===-------------------- debug_main.cpp ------------------------*- C++ -*-===//
// Used for debugging.
//
// prefix: debug_
//
// OUTPUTS:
//   debug_task1000ms_watermark
//   debug_task200ms_watermark
//===----------------------------------------------------------------------===//

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

extern TaskHandle_t xTaskHandle_200ms;
extern TaskHandle_t xTaskHandle_1000ms;

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
static size_t debug_task1000ms_watermark;
static SemaphoreHandle_t mutex_debug_task1000ms_watermark;

static size_t debug_task200ms_watermark;
static SemaphoreHandle_t mutex_debug_task200ms_watermark;

// Set
static void set_debug_task_1000ms_watermark(const size_t *pWatermark) {
  if (xSemaphoreTake(mutex_debug_task1000ms_watermark,
                     100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(&debug_task1000ms_watermark, pWatermark, 1);
    xSemaphoreGive(mutex_debug_task1000ms_watermark);
  }
}

static void set_debug_task_200ms_watermark(const size_t *pWatermark) {
  if (xSemaphoreTake(mutex_debug_task1000ms_watermark,
                     100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(&debug_task200ms_watermark, pWatermark, 1);
    xSemaphoreGive(mutex_debug_task200ms_watermark);
  }
}

// Get
void get_debug_task_1000ms_watermark(size_t *pWatermark) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_debug_task1000ms_watermark,
                     100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(pWatermark, &debug_task1000ms_watermark, 1);
    xSemaphoreGive(mutex_debug_task1000ms_watermark);
  }
}

// Get
void get_debug_task_200ms_watermark(size_t *pWatermark) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_debug_task200ms_watermark,
                     100 / portTICK_PERIOD_MS) == pdTRUE) {
    memcpy(pWatermark, &debug_task200ms_watermark, 1);
    xSemaphoreGive(mutex_debug_task200ms_watermark);
  }
}

// Init
void debug_init(void) {
  debug_task1000ms_watermark = 0;
  debug_task200ms_watermark = 0;
  mutex_debug_task1000ms_watermark = xSemaphoreCreateMutex();
  mutex_debug_task200ms_watermark = xSemaphoreCreateMutex();
}

// ------- Actual function starts here! -------------
void debug_main() {

  size_t task_200ms_watermark;
  task_200ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_200ms);
  size_t task_1000ms_watermark;
  task_1000ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_1000ms);

  // OUTPUT
  set_debug_task_1000ms_watermark(&task_1000ms_watermark);
  set_debug_task_200ms_watermark(&task_200ms_watermark);
}
