//===-------------------- debug.c ------------------------*- C -*-===//
// Used for debugging. You just check the watermarks.
//
// PREFIX: debug_
// PUBLISHED SIGNALS: None.
//===----------------------------------------------------------------------===//

#include "FreeRTOS.h"
#include "application_setup.h"
#include <semphr.h>
#include <string.h>

#define NOOP ({ ; })
extern TaskHandle_t xTaskHandle_200ms;
extern TaskHandle_t xTaskHandle_1000ms;
extern TaskHandle_t xTaskBuitinButtonDeferred;
extern TaskHandle_t xTaskUsart2RxDeferred;


/************
 *  INPUTS  *
 ************/
/* None */


/************
 *  OUTPUTS *
 ************/
/* None */


/******************
 *  INIT FUNCTION *
 *****************/
void debug_init(void) {}

/******************
 *  STEP FUNCTION *
 ******************/
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

  volatile size_t task_200ms_watermark;
  task_200ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_200ms);
  volatile size_t task_1000ms_watermark;
  task_1000ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_1000ms);
  volatile size_t xTaskBuitinButtonDeferred_watermark;
  xTaskBuitinButtonDeferred_watermark =
      uxTaskGetStackHighWaterMark(xTaskBuitinButtonDeferred);
  volatile size_t xTaskUsart2RxDeferred_watermark;
  xTaskUsart2RxDeferred_watermark =
      uxTaskGetStackHighWaterMark(xTaskUsart2RxDeferred);

  NOOP;
  /* volatile size_t task_200ms_watermark; */
  /* task_200ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_200ms); */
  /* volatile size_t task_1000ms_watermark; */
  /* task_1000ms_watermark = uxTaskGetStackHighWaterMark(xTaskHandle_1000ms); */
}
