
#include "serial_port.h"
#include "hw_config_arduino.h"
#include <semphr.h>

static SemaphoreHandle_t mutex_SerialPortQueue;

const size_t MESSAGE_SIZE_MAX = 30;
const size_t QUEUE_LENGTH = 5;
static QueueHandle_t xSerialPortQueue;

// OUTPUTS
static char serial_data[MESSAGE_SIZE_MAX];
static SemaphoreHandle_t mutex_serial_data;

// Set
static void serial_set_data(const char *pVal) {
  bool isSemaphoreTaken;
  isSemaphoreTaken =
      xSemaphoreTake(mutex_serial_data, 100 / portTICK_PERIOD_MS) == pdTRUE;
  if (isSemaphoreTaken) {
    strcpy(serial_data, pVal);
    xSemaphoreGive(mutex_serial_data);
  }
}

// Get
char *serial_get_data() {
  // TODO This shall be READ ONLY!
  /* char message[] = "Error!"; */
  /* if (xSemaphoreTake(mutex_serial_data, 100 / portTICK_PERIOD_MS) == pdTRUE)
   * { */
  /*   strcpy(message, serial_data); */
  /*   xSemaphoreGive(mutex_serial_data); */
  /* } */
  // TODO what if someone is still writing on the serial port?
  return serial_data;
}
// Use myQueue in functions as needed
void serial_port_init() {

  // Create Serial queue and associated mutex
  xSerialPortQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE_MAX);
  mutex_SerialPortQueue = xSemaphoreCreateMutex();
}

// This function can be used by anyone in any task to send stuff to the serial
// port.
void serial_port_send(const char *pMessage) {

  Serial.println(strlen(pMessage));
  boolean isSerialPortQueueAvailable;
  /* isSerialPortQueueAvailable = */
  /*     xSemaphoreTake(mutex_SerialPortQueue, 100 / portTICK_PERIOD_MS) ==
   * pdTRUE; */

  /* if (isSerialPortQueueAvailable) { */
  /*   if (uxQueueSpacesAvailable(xSerialPortQueue) > 0) { */
  /*     xQueueSend(xSerialPortQueue, pMessage, 100 / portTICK_PERIOD_MS); */
  /*   } */
  /*   xSemaphoreGive(mutex_SerialPortQueue); */
  /* } */
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
        serial_set_data(received_data);
      }
      xSemaphoreGive(mutex_SerialPortQueue);
    }
  }
}
