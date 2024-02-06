//===-------------------- debug_step.c ------------------------*- C -*-===//
// Used for debugging.
//
// prefix: debug_
//
//===----------------------------------------------------------------------===//

#include "FreeRTOS.h"
#include <semphr.h>
#include <string.h>
#include "application_setup.h"

#define NOOP ({ ; })
extern TaskHandle_t xTaskHandle_200ms;
extern TaskHandle_t xTaskHandle_1000ms;

// Init
void debug_init(void) {}

// ------- Actual function starts here! -------------
void debug_step(enum WhoIsCalling caller) {
    (void)caller;

  TaskStatus_t xTaskDetails_1000ms;
  /* Check the handle is not NULL. */
  configASSERT(xTaskHandle_1000ms);
  /* Use the handle to obtain further information about the task. */
  vTaskGetInfo(xTaskHandle_1000ms, &xTaskDetails_1000ms, pdTRUE, eInvalid);

  volatile UBaseType_t number_1000ms = xTaskDetails_1000ms.xTaskNumber;
  volatile eTaskState state_1000ms = xTaskDetails_1000ms.eCurrentState;
  volatile configSTACK_DEPTH_TYPE watermark_1000ms =
      xTaskDetails_1000ms.usStackHighWaterMark;
  volatile UBaseType_t priority_1000ms = xTaskDetails_1000ms.uxCurrentPriority;

  NOOP;

  TaskStatus_t xTaskDetails_200ms;
  /* Check the handle is not NULL. */
  configASSERT(xTaskHandle_200ms);
  /* Use the handle to obtain further information about the task. */
  vTaskGetInfo(xTaskHandle_200ms, &xTaskDetails_200ms, pdTRUE, eInvalid);

  volatile UBaseType_t number_200ms = xTaskDetails_200ms.xTaskNumber;
  volatile eTaskState state_200ms = xTaskDetails_200ms.eCurrentState;
  volatile configSTACK_DEPTH_TYPE watermark_200ms =
      xTaskDetails_200ms.usStackHighWaterMark;
  volatile UBaseType_t priority_200ms = xTaskDetails_200ms.uxCurrentPriority;

  NOOP;
  /* volatile size_t task_200ms_watermark; */
  /* task_200ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_200ms); */
  /* volatile size_t task_1000ms_watermark; */
  /* task_1000ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_1000ms); */
}
