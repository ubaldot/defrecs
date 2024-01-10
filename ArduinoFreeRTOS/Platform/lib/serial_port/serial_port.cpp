
#include "serial_port.h"
#include "hw_config_arduino.h"
#include "pinout_functions_arduino.h"
#include <semphr.h>

static SemaphoreHandle_t mutex_SerialPortQueue;
static QueueHandle_t xSerialPortQueue;

// OUTPUTS AS EXAMPLES OF SETTER AND GETTER METHODS
/* static char received_data[MESSAGE_SIZE_MAX]; */
/* static SemaphoreHandle_t mutex_serial_data; */

// Set
/* static void serial_set_data(const char *pMessage) { */
/*   if (xSemaphoreTake(mutex_serial_data, 100 / portTICK_PERIOD_MS) == pdTRUE)
 * { */
/*     strcpy(serial_data, pMessage); */
/*     xSemaphoreGive(mutex_serial_data); */
/*   } */
/* } */

// Get
/* void serial_get_data(char *pMessage) { */
/*   // TODO This shall be READ ONLY! */
/*   if (xSemaphoreTake(mutex_serial_data, 100 / portTICK_PERIOD_MS) == pdTRUE) { */
/*     strcpy(pMessage, received_data); */
/*     xSemaphoreGive(mutex_serial_data); */
/*   } */
/* } */
// Use myQueue in functions as needed
void serial_port_init() {

  // Create Serial queue and associated mutex
  xSerialPortQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE_MAX);
  mutex_SerialPortQueue = xSemaphoreCreateMutex();
}

// This function can be used by anyone in any task to send stuff to the serial
// port.
void serial_port_send(const char *pMessage) {
  // push messages to the queue
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
  // pop messages from the queue (if any) and set component's output
  char received_data[MESSAGE_SIZE_MAX];
  // If any message available
  if (uxQueueMessagesWaiting(xSerialPortQueue) > 0) {
    // Lock the Serial Queue to extract data

    if (xSemaphoreTake(mutex_SerialPortQueue, 100 / portTICK_PERIOD_MS) ==
        pdTRUE) {
      BaseType_t isReceived;
      isReceived = xQueueReceive(xSerialPortQueue, &received_data,
                                 100 / portTICK_PERIOD_MS);
      if (isReceived == pdPASS) {
        pinout_serial_port(received_data);
      }
      xSemaphoreGive(mutex_SerialPortQueue);
    }
  }
}
