//===------------------ interrupts_to_tasks.c-------------------*- C -*-===//
// Each interrupt is deferred to a task with a reasonably high priority.
//
//    IRQ -> ISR -> Callback -> Task woken up.
//
// ISR callbacks do only one thing: notify a task that wakes up.
// Such a task may be a periodic task in case of predictable events and a
// deferring task in case of unpredictable event (see serial_port component).
//
// Here, only deferring tasks are defined. As you see, they both handle
// unpredictable events.
// ISR, callbacks, etc. are declared in  Core/Src/stm32fxx_it.c.
//
//===----------------------------------------------------------------------===//
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart2/usart2.h"

void BuiltinButtonDeferred(void * /*pVParameters*/);

// Builtin button pressed. Only tasks associated to unpredictable events shall
// be defined here.
TaskHandle_t xTaskBuitinButtonDeferred;

// Init
void interrupts_to_tasks_init() {
  /* Transmit upon button press */
  xTaskCreate(BuiltinButtonDeferred, "BuiltinButtonPressed", 128, NULL,
              configMAX_PRIORITIES - 1, &xTaskBuitinButtonDeferred);
}

// Functions associated to tasks
void BuiltinButtonDeferred(void *pVParameters) {
  (void)pVParameters;

  uint32_t ulEventsToProcess;

  for (;;) {
    /* Wait to receive a notification sent directly to this task from the
     interrupt service routine. */
    ulEventsToProcess = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));
    if (ulEventsToProcess != 0) {
      /* To get here at least one event must have occurred. Loop here
      until all the pending events have been processed (in this case,
      just print out a message for each event). */
      while (ulEventsToProcess > 0) {
        usart2_step(IRQ_BUILTIN_BUTTON);
        ulEventsToProcess--;
      }
    } else {
      /* If this part of the function is reached then an interrupt did
      not arrive within the expected time, and (in a real application)
      it may be necessary to perform some error recovery operations. */
    }
  }
}
