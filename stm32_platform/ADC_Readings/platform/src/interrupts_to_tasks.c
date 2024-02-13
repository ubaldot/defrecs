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
// prefix: irq_
//
// PUBLISHED SIGNALS:
//   - irq_builtin_button_message
//   - irq_raw_rx_message
//===----------------------------------------------------------------------===//
//
#include "FreeRTOS.h"
#include "pinin.h"
#include "semphr.h"
#include "serial_port/serial_port.h"
#include <string.h>

void BuiltinButtonDeferred(void * /*pVParameters*/);
void Usart2RxDeferred(void * /*pVParameters*/);

// Builtin button pressed
TaskHandle_t xTaskBuitinButtonDeferred;
extern SemaphoreHandle_t xSemaphoreBuiltinButton;

// Received message over the serial port
TaskHandle_t xTaskUsart2RxDeferred;
extern SemaphoreHandle_t xSemaphoreUsart2Rx;

// Aux for UART reception.
static uint8_t tmp_rx_buffer[MSG_LENGTH_MAX];
static size_t ii; // For counting the number of bytes received
static SemaphoreHandle_t mutex_tmp_rx_buffer;

// Published signals
static uint8_t irq_raw_rx_message[MSG_LENGTH_MAX];
static uint8_t irq_builtin_button_message[MSG_LENGTH_MAX];

static SemaphoreHandle_t mutex_irq_raw_rx_message;
static SemaphoreHandle_t mutex_irq_builtin_button;

// Publish functions
static void publish_irq_raw_rx_message(const uint8_t *pRawMessage,
                                       size_t arrlen) {
  if (xSemaphoreTake(mutex_irq_raw_rx_message, pdMS_TO_TICKS(5)) == pdTRUE) {
    memcpy(irq_raw_rx_message, pRawMessage, arrlen);
    irq_raw_rx_message[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_raw_rx_message);
  }
}

static void publish_irq_builtin_button_message(const uint8_t *pRawMessage,
                                               size_t arrlen) {
  if (xSemaphoreTake(mutex_irq_builtin_button, pdMS_TO_TICKS(5)) == pdTRUE) {
    memcpy(irq_builtin_button_message, pRawMessage, arrlen);
    irq_builtin_button_message[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_builtin_button);
  }
}

// Subscribe functions
void subscribe_irq_raw_rx_message(uint8_t *pRawMessage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_irq_raw_rx_message, pdMS_TO_TICKS(5)) == pdTRUE) {
    size_t arrlen = 0;
    while (irq_raw_rx_message[arrlen] != '\0') {
      arrlen++;
    }
    memcpy(pRawMessage, irq_raw_rx_message, arrlen);
    pRawMessage[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_raw_rx_message);
  }
}

void subscribe_irq_builtin_button_message(uint8_t *pRawMessage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_irq_builtin_button, pdMS_TO_TICKS(5)) == pdTRUE) {
    size_t arrlen = 0;
    while (irq_builtin_button_message[arrlen] != '\0') {
      arrlen++;
    }
    memcpy(pRawMessage, irq_builtin_button_message, arrlen);
    pRawMessage[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_builtin_button);
  }
}
// Init
void interrupts_to_tasks_init() {
  /* Transmit upon button press */
  xSemaphoreBuiltinButton = xSemaphoreCreateBinary();
  xTaskCreate(BuiltinButtonDeferred, "Usart2Tx", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskBuitinButtonDeferred);

  /* Receive data on usart2*/
  xSemaphoreUsart2Rx = xSemaphoreCreateBinary();
  xTaskCreate(Usart2RxDeferred, "Usart2Rx", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskUsart2RxDeferred);

  /* mutex for published signals */
  mutex_irq_raw_rx_message = xSemaphoreCreateMutex();
  mutex_irq_builtin_button = xSemaphoreCreateMutex();

  /* Usart init */
  ii = 0;
  mutex_tmp_rx_buffer = xSemaphoreCreateMutex();
  pinin_usart(tmp_rx_buffer, 1, INTERRUPT); // Initialize for reception
}

// Functions associated to tasks
void BuiltinButtonDeferred(void *pVParameters) {
  (void)pVParameters;
  const uint8_t MESSAGE[] = "Button pressed!\r\n";

  size_t arrlen = 0;
  while (MESSAGE[arrlen] != '\0') {
    arrlen++;
  }
  publish_irq_builtin_button_message(MESSAGE, arrlen);

  for (;;) {
    while (xSemaphoreTake(xSemaphoreBuiltinButton, portMAX_DELAY) == pdTRUE) {
      serial_port_step(IRQ_BUILTIN_BUTTON);
    }
  }
}

void Usart2RxDeferred(void *pVParameters) {
  (void)pVParameters;

  for (;;) {
    while (xSemaphoreTake(xSemaphoreUsart2Rx, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(mutex_tmp_rx_buffer, pdMS_TO_TICKS(5)) == pdTRUE) {
        if (tmp_rx_buffer[ii] != '\r' && ii++ < MSG_LENGTH_MAX) {
          pinin_usart(&tmp_rx_buffer[ii], 1, INTERRUPT);
        } else {
          publish_irq_raw_rx_message(tmp_rx_buffer, ii);
          serial_port_step(IRQ_SERIAL_RX);

          /* Reinitialize all the variables used */
          ii = 0;
          tmp_rx_buffer[0] = '\0';
          irq_raw_rx_message[0] = '\0';
          pinin_usart(&tmp_rx_buffer[ii], 1, INTERRUPT);
        }
        xSemaphoreGive(mutex_tmp_rx_buffer);
      }
    }
  }
}
