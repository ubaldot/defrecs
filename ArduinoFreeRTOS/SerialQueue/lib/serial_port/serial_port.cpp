
#include "serial_port.h"
#include <semphr.h>
#include "hw_config.h"

static SemaphoreHandle_t mutex_SerialPortQueue;
static QueueHandle_t xSerialPortQueue;

// Use myQueue in functions as needed
void serial_port_init() {

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;
  };
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
  // If any message available
  if (uxQueueMessagesWaiting(xSerialPortQueue) != 0U) {
    // Lock the Serial Queue to extract data
    boolean isSerialPortQueueTaken;
    isSerialPortQueueTaken = xSemaphoreTake(mutex_SerialPortQueue,
                                            100 / portTICK_PERIOD_MS) == pdTRUE;
    if (isSerialPortQueueTaken) {
      BaseType_t isReceived;
      isReceived = xQueueReceive(xSerialPortQueue, &received_data,
                                 100 / portTICK_PERIOD_MS);
      if (isReceived == pdPASS) {
        for (size_t ii = 0; ii < strlen(received_data) - 1; ii++) {
          Serial.print((char)received_data[ii]);
        }
        Serial.print("\n");
      }
      xSemaphoreGive(mutex_SerialPortQueue);
    }
  }
}
