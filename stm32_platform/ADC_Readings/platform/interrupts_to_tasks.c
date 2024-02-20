//===------------------ interrupts_to_tasks.c-------------------*- C -*-===//
// Each interrupt is deferred to a task with a reasonably high priority.
//
//    Process: IRQ -> ISR -> Callback -> Task woken up.
//
// ISR callbacks do only one thing: release a semaphore that unlock a task.
// They are defined in Core/Src/stmf4xx_it.c file.
//
// Next, you have two possibilities for handling stuff once the Callback is
// called:
//   1. You want to update something in a component scheduled periodically and
//   you are fine to wait for the next component execution,
//   2. You want to update something and call the component immediately.
//
// Let' see these two cases.
//
// Case 1.
// ------
//   In your periodically called component, you can have something like
//     if (xSemaphoreTake(xSemaphoreFromSpecificISRCallback, delay)){
//        // update something
//     }else{
//        // Do the normal periodic task stuff
//     }
//   The above happens for example with ADC for the Photovoltaic in DMA mode in
//   pinin.
//   You could or could not publish any signal in this case.
//
// Case 2.
// ------
//   You do two things:
//     a) Publish a signal in this file,
//     b) Immediately call the components subscribed to the published signal.
//
//   In that case, you switch case based on the WhoIsCalling parameter, see
//   serial_port component.
//
// ISR, callbacks, semaphores, etc. are declared in // Core/Src/stm32fxx_it.c.
//
//===----------------------------------------------------------------------===//
//
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart2/usart2.h"

void BuiltinButtonDeferred(void * /*pVParameters*/);
void Usart2RxDeferred(void * /*pVParameters*/);

// Builtin button pressed
TaskHandle_t xTaskBuitinButtonDeferred;
extern SemaphoreHandle_t xSemaphoreBuiltinButton;

// Received message over the serial port
TaskHandle_t xTaskUsart2RxDeferred;
extern SemaphoreHandle_t xSemaphoreUsart2Rx;

// Init
void interrupts_to_tasks_init() {
  /* Transmit upon button press */
  xSemaphoreBuiltinButton = xSemaphoreCreateBinary();
  xTaskCreate(BuiltinButtonDeferred, "BuiltinButtonPressed", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskBuitinButtonDeferred);

  /* Receive data on usart2*/
  xSemaphoreUsart2Rx = xSemaphoreCreateBinary();
  xTaskCreate(Usart2RxDeferred, "Usart2Rx", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskUsart2RxDeferred);
}

// Functions associated to tasks
void BuiltinButtonDeferred(void *pVParameters) {
  (void)pVParameters;

  for (;;) {
    xSemaphoreTake(xSemaphoreBuiltinButton, portMAX_DELAY);
    usart2_step(IRQ_BUILTIN_BUTTON);
  }
}

void Usart2RxDeferred(void *pVParameters) {
  (void)pVParameters;
  for (;;) {
    xSemaphoreTake(xSemaphoreUsart2Rx, portMAX_DELAY);
    usart2_step(IRQ_SERIAL_RX);
  }
}
