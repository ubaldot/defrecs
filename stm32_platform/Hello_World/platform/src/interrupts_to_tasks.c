/* ALAALALAL */
/* Deferred tasks from ISR calls. */
/* Each these tasks defined here is blocked and it is woken up when its
 * associated ISR release the semaphore. */

#include "FreeRTOS.h"
#include "pinin.h"
#include "semphr.h"
#include "serial_port/serial_port.h"
#include <string.h>

void BuiltinButton(void * /*pVParameters*/);
void Usart2RxDeferred(void * /*pVParameters*/);

// Builtin button pressed
TaskHandle_t xTaskBuitinButtonDeferred;
SemaphoreHandle_t xSemaphoreBuiltinButton;

// Received message over the serial port
TaskHandle_t xTaskUsart2RxDeferred;
SemaphoreHandle_t xSemaphoreUsart2Rx;
static size_t ii = 0; // For counting the number of bytes received
static uint8_t tmp_rx_buffer[MSG_LENGTH_MAX];
static SemaphoreHandle_t mutex_tmp_rx_buffer;

// OUTPUTS
static uint8_t irq_raw_rx_message[MSG_LENGTH_MAX];
static uint8_t irq_builtin_button[MSG_LENGTH_MAX];

static SemaphoreHandle_t mutex_irq_raw_rx_message;
static SemaphoreHandle_t mutex_irq_builtin_button;

// Publish
static void publish_irq_raw_rx_message(const uint8_t *pRawMessage,
                                       size_t arrlen) {
  // The mutex is already taken.
  if (xSemaphoreTake(mutex_irq_raw_rx_message, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(irq_raw_rx_message, pRawMessage, arrlen);
    irq_raw_rx_message[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_raw_rx_message);
  }
}

static void publish_irq_builtin_button(const uint8_t *pRawMessage,
                                       size_t arrlen) {
  if (xSemaphoreTake(mutex_irq_builtin_button, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(irq_builtin_button, pRawMessage, arrlen);
    irq_builtin_button[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_builtin_button);
  }
}

// Subscribe (for the others)
void subscribe_irq_raw_rx_message(uint8_t *pRawMessage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_irq_raw_rx_message, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    size_t arrlen = 0;
    while (irq_raw_rx_message[arrlen] != '\0') {
      arrlen++;
    }
    memcpy(pRawMessage, irq_raw_rx_message, arrlen);
    pRawMessage[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_raw_rx_message);
  }
}

void subscribe_irq_builtin_button(uint8_t *pRawMessage) {
  // Returns a copy of the output
  if (xSemaphoreTake(mutex_irq_builtin_button, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    size_t arrlen = 0;
    while (irq_builtin_button[arrlen] != '\0') {
      arrlen++;
    }
    memcpy(pRawMessage, irq_builtin_button, arrlen);
    pRawMessage[arrlen] = '\0';
    xSemaphoreGive(mutex_irq_builtin_button);
  }
}

void interrupts_to_tasks_init() {
  /* Transmit upon button press */
  xSemaphoreBuiltinButton = xSemaphoreCreateBinary();
  xTaskCreate(BuiltinButton, "Usart2Tx", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskBuitinButtonDeferred);

  /* Receive data */
  xSemaphoreUsart2Rx = xSemaphoreCreateBinary();
  xTaskCreate(Usart2RxDeferred, "Usart2Rx", 128, NULL,
              configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
              &xTaskUsart2RxDeferred);

  ii = 0;
  mutex_irq_raw_rx_message = xSemaphoreCreateMutex();
  mutex_irq_builtin_button = xSemaphoreCreateMutex();
  mutex_tmp_rx_buffer = xSemaphoreCreateMutex();
  pinin_usart(tmp_rx_buffer, 1, INTERRUPT); // Initialize for reception
}

void BuiltinButton(void *pVParameters) {
  (void)pVParameters;
  const uint8_t MESSAGE[] = "Leccami la nerchia.\r\n";

  size_t arrlen = 0;
  while (MESSAGE[arrlen] != '\0') {
    arrlen++;
  }
  publish_irq_builtin_button(MESSAGE, arrlen);

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
      if (xSemaphoreTake(mutex_tmp_rx_buffer, 100 / portTICK_PERIOD_MS) ==
          pdTRUE) {
        if (tmp_rx_buffer[ii] != '\r' && ii++ < MSG_LENGTH_MAX) {
          pinin_usart(&tmp_rx_buffer[ii], 1, INTERRUPT);
        } else {
          publish_irq_raw_rx_message(tmp_rx_buffer, ii);
          serial_port_step(IRQ_SERIAL_RX);
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
