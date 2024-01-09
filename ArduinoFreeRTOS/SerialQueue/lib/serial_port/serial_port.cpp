
#include "serial_port.h"
#include <semphr.h>

static SemaphoreHandle_t mutex_SerialPortQueue;
static QueueHandle_t xSerialPortQueue;

// Use myQueue in functions as needed
void serial_port_init() {

  // Create Serial queue and associated mutex
  xSerialPortQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE_MAX);
  mutex_SerialPortQueue = xSemaphoreCreateMutex();
}

// This function can be used by anyone in any task to send stuff to the serial
// port.
void serial_port_send(const char *pMessage) {

  boolean isSerialPortQueueAvailable;
  isSerialPortQueueAvailable =
      xSemaphoreTake(mutex_SerialPortQueue, 100 / portTICK_PERIOD_MS) == pdTRUE;

  if (isSerialPortQueueAvailable) {
    if (uxQueueSpacesAvailable(xSerialPortQueue) > 0) {
      xQueueSend(xSerialPortQueue, pMessage, 100 / portTICK_PERIOD_MS);
    }
    xSemaphoreGive(mutex_SerialPortQueue);
  }
}

// This function is the main. If there is anything in the queue it just send
// it to the serial port.
void serial_port_main() {
  char received_data[MESSAGE_SIZE_MAX];
  if (uxQueueMessagesWaiting(xSerialPortQueue) != 0U) {
    BaseType_t isReceived;
    isReceived = xQueueReceive(xSerialPortQueue, &received_data,
                               1000 / portTICK_PERIOD_MS);
    /* Serial.println(*received_data); */
    if (isReceived == pdPASS) {
      Serial.println(*received_data);
    }
  }
}
