#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>
// Global variables: semaphore and message buffer
/* Create a message buffer to send data to the serial*/
const size_t MESSAGE_SIZE = 100;
MessageBufferHandle_t xMessageBuffer = xMessageBufferCreate(MESSAGE_SIZE);

/* Create a semaphore to control access to the message buffer */
// TODO: do we really need static?
static SemaphoreHandle_t xSemaphoreMessageBuffer = xSemaphoreCreateBinary();
static SemaphoreHandle_t xSemaphoreSerialPort = xSemaphoreCreateMutex();

// Task functions prototypes
void task_blink(void *pVParameters);
void task_serial(void *pVParameters);

void setup() {

  // initialize serial communication at 9600 bits per second:
  const unsigned int BAUDRATE = 9600;
  Serial.begin(BAUDRATE);

  // TODO: is it really needed?
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO,
  }

  // Release all the semaphore at the very beginning.
  xSemaphoreGive(xSemaphoreMessageBuffer);
  xSemaphoreGive(xSemaphoreSerialPort);

  // Task: Blink
  const size_t STACK_SIZE_BLINK = 128;
  const uint8_t PRIORITY_BLINK = 2;
  TaskHandle_t xTaskHandleBlink;

  xTaskCreate(task_blink, "Blink", STACK_SIZE_BLINK, NULL, PRIORITY_BLINK,
              &xTaskHandleBlink);

  // Task: Serial
  const size_t STACK_SIZE_SERIAL = 512;
  const uint8_t PRIORITY_SERIAL = 2;
  TaskHandle_t xTaskHandleSerial;

  xTaskCreate(task_serial, "Serial", STACK_SIZE_SERIAL, NULL, PRIORITY_SERIAL,
              &xTaskHandleSerial);
}
// Skip loop. OBS! IdleTask cannot be used!
void loop() {}

// Task implementation
void task_blink(void *pVParameters) // This is a task.
{
  (void)pVParameters;
  const int TASK_PERIOD = 1000;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  static uint8_t counter = 0;
  const size_t MAX_COUNT = 2;

  // Condition for sending a message over the serial port
  boolean isMessageBufferSemaphoreTaken;
  boolean isMessageShortEnough;

  while (true) {
    static bool led_state = false;
    // send something every MAX_COUNT*TASK_PERIOD seconds.

    if (!led_state) {
      digitalWrite(LED_BUILTIN, HIGH);
      led_state = true;
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      led_state = false;
    }

    char message[] = "dog";
    if (counter == MAX_COUNT) {

      // Debug
      Serial.print("Available space in message buffer: ");
      Serial.println(xMessageBufferSpacesAvailable((xMessageBuffer)));

      // Check if you can send a message
      isMessageBufferSemaphoreTaken =
          xSemaphoreTake(xSemaphoreMessageBuffer, 1000 / portTICK_PERIOD_MS) ==
          pdTRUE;
      isMessageShortEnough =
          xMessageBufferSpacesAvailable((xMessageBuffer)) > strlen(message);

      if (isMessageBufferSemaphoreTaken && isMessageShortEnough) {
        xMessageBufferSend(xMessageBuffer, message, sizeof(message), 0);
        Serial.println("Message sent!");
        xSemaphoreGive(xSemaphoreMessageBuffer);
      }
      counter = 0;
    } else {
      counter++;
    }

    // Task Schedule
    const TickType_t X_DELAY = TASK_PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}

void task_serial(void *pVParameters) // This is a task.
{
  (void)pVParameters;
  const uint8_t TASK_PERIOD = 200;
  uint8_t received_data[MESSAGE_SIZE];
  size_t received_bytes;
  received_bytes = 0;

  boolean isMessageBufferSemaphoreTaken;
  boolean isSerialSemaphoreTaken;
  boolean isMessageBufferEmpty;

  while (true) {
    // Conditions for writing on the serial

    isSerialSemaphoreTaken =
        xSemaphoreTake(xSemaphoreSerialPort, 1000 / portTICK_PERIOD_MS) ==
        pdTRUE;
    isMessageBufferSemaphoreTaken =
        xSemaphoreTake(xSemaphoreMessageBuffer, 1000 / portTICK_PERIOD_MS) ==
        pdTRUE;
    isMessageBufferEmpty = xMessageBufferIsEmpty(xMessageBuffer) == pdTRUE;

    if (isSerialSemaphoreTaken) {
      if (isMessageBufferSemaphoreTaken) {
        if (!isMessageBufferEmpty) {
          /* Serial.println("TEST"); */

          /* ------------- BEGIN NON-WORKING PART!!! -----------*/
          received_bytes = xMessageBufferReceive(xMessageBuffer, received_data,
                                                 sizeof(received_data), 0);
          Serial.print("received_bytes: ");
          Serial.println(received_bytes);
          for (size_t ii = 0; ii < received_bytes -1; ii++) {
            Serial.print((char)received_data[ii]);
          }
          Serial.print("\n");
          /* xMessageBufferReset(xMessageBuffer); */
          /* ------------- END NON-WORKING PART!!! -----------*/
        }
        xSemaphoreGive(xSemaphoreMessageBuffer);
        /* Serial.println("(serial) Semaphore MessageBuffer released."); */
      }
      xSemaphoreGive(xSemaphoreSerialPort);
      /* Serial.println("(serial) Semaphore serial port released."); */
    }

    // TODO: Task Schedule
    const TickType_t X_DELAY = TASK_PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}
