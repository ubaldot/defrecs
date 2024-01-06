
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>

// {P,I,D} + (float) val
const unsigned int K_MAX_MESSAGE_LENGTH = 5;

// Received from serialplot
union Message {
  struct {
    int gain_type;
    float gain_value;
  } data;
  char bytes[K_MAX_MESSAGE_LENGTH];
} message;

//
// Global variables: semaphore and message buffer
/* Create a message buffer to send data to the serial*/
const size_t MESSAGE_SIZE = 100;
MessageBufferHandle_t xMessageBuffer;

/* Create a semaphore to control access to the message buffer */
// TODO: CHECK if the semaphore is created.
SemaphoreHandle_t xSemaphoreMessageBuffer;
SemaphoreHandle_t xSemaphoreSerialPort;

// Task functions prototypes
void task_blink(void *pVParameters);
void task_controller(void *pVParameters);
void task_serial(void *pVParameters);

// Aux functions
void set_pid_gains(float * /*Kp*/, float * /*Ki*/, float * /*Kd*/);
void get_serial_message(union Message * /*message*/);

// the setup function runs once when you press reset or power the board
void setup() {

  // SERIAL: initialize serial communication at 9600 bits per second:
  const uint16_t BAUDRATE = 9600;
  Serial.begin(BAUDRATE);

  // TODO: needed?
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO,
  }

  // Message buffer creation
  xMessageBuffer = xMessageBufferCreate(MESSAGE_SIZE);

  if (xMessageBuffer == NULL) {
    Serial.println("I cannot create a Message Buffer.");
  }

  // Create semaphores
  xSemaphoreMessageBuffer = xSemaphoreCreateBinary();
  if (xSemaphoreMessageBuffer == NULL) {
    Serial.println("I cannot create a semaphore for Message Buffer.");
  }
  xSemaphoreSerialPort = xSemaphoreCreateMutex();
  if (xSemaphoreSerialPort == NULL) {
    Serial.println("I cannot create a semaphore for the Serial port.");
  }

  // Release all the semaphore at the very beginning.
  xSemaphoreGive(xSemaphoreMessageBuffer);
  xSemaphoreGive(xSemaphoreSerialPort);

  /* Serial.println("--- FreeRTOS semaphore example ---"); */

  // Task: Blink
  const size_t STACK_SIZE_BLINK = 128;
  const uint8_t PRIORITY_BLINK = 2;
  BaseType_t xReturnedBlink;
  TaskHandle_t xTaskHandleBlink;

  xReturnedBlink = xTaskCreate(task_blink, "Blink", STACK_SIZE_BLINK, NULL,
                               PRIORITY_BLINK, &xTaskHandleBlink);
  // TODO:
  if (xReturnedBlink != pdPASS) {
    Serial.println("I cannot instantiate the Blink task.");
    // Do something if the task cannot be allocated
  }

  // Task: Controller
  /* const int STACK_SIZE_CONTROLLER = 256; */
  /* const int PRIORITY_CONTROLLER = 2; */
  /* BaseType_t xReturnedController; */
  /* TaskHandle_t xTaskHandleController; */

  // Controller
  /* xReturnedController = */
  /*     xTaskCreate(task_controller, "Controller", STACK_SIZE_CONTROLLER, NULL,
   */
  /*                 PRIORITY_CONTROLLER, &xTaskHandleController); */

  /* if (xReturnedController != pdPASS) { */
  /* } */

  // Task: Serial
  const int STACK_SIZE_SERIAL = 256;
  const int PRIORITY_SERIAL = 2;
  BaseType_t xReturnedSerial;
  TaskHandle_t xTaskHandleSerial;

  xReturnedSerial = xTaskCreate(task_serial, "Serial", STACK_SIZE_SERIAL, NULL,
                                PRIORITY_SERIAL, &xTaskHandleSerial);

  if (xReturnedSerial != pdPASS) {
    Serial.println("I cannot instantiate the Serial task.");
  }
}
// Skip loop. OBS! IdleTask cannot be used!
void loop() {}

// Task implementation
void task_blink(void *pVParameters) // This is a task.
{
  (void)pVParameters;
  const uint16_t TASK_PERIOD = 1000;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  static uint8_t counter = 0;
  const uint8_t MAX_COUNT = 2;

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
      /* Serial.print("Available space in message buffer: "); */
      /* Serial.println(xMessageBufferSpacesAvailable((xMessageBuffer))); */

      // Check if you can send a message
      isMessageBufferSemaphoreTaken =
          xSemaphoreTake(xSemaphoreMessageBuffer, 1000 / portTICK_PERIOD_MS) ==
          pdTRUE;
      isMessageShortEnough =
          xMessageBufferSpacesAvailable((xMessageBuffer)) > strlen(message);

      // Debug
      /* Serial.print("cond 1 (is semaphore taken): "); */
      /* Serial.println(static_cast<int>(isMessageBufferSemaphoreTaken)); */
      /* Serial.print("cond 2 (is available space): "); */
      /* Serial.println(static_cast<int>(isMessageShortEnough)); */
      if (isMessageBufferSemaphoreTaken && isMessageShortEnough) {
        /* Serial.println("SESSO"); */
        xMessageBufferSend(xMessageBuffer, message, sizeof(message), 0);
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

/* void task_controller(void *pVParameters) // This is a task. */
/* { */
/*   (void)pVParameters; */
/*   const int TASK_PERIOD = 200; */

/*   static unsigned int counter = 0; */
/*   const unsigned int MAX_COUNT = 3; */
/*   // PWN on pin 5 */
/*   const int FAN_PIN = 5; */
/*   pinMode(FAN_PIN, OUTPUT); */

/*   while (true) { */
/*     int sensor_value; */
/*     sensor_value = analogRead(A0); */
/*     // Controller return a 0-1024, whereas analogWrite is 0-255 */
/*     // Controller has gain 1 */
/*     static float kp; */
/*     static float ki; */
/*     static float kd; */
/*     set_pid_gains(&kp, &ki, &kd); */
/*     int pwm_value; */
/*     pwm_value = sensor_value >> 2; */
/*     Serial.println(pwm_value); */
/*     analogWrite(FAN_PIN, pwm_value); */

/*     char message[] = "penguin"; */
/*     if (counter == MAX_COUNT) { */
/*       if (xSemaphoreTake(xSemaphoreMessageBuffer, portMAX_DELAY) == pdTRUE &&
 */
/*           (xMessageBufferSpacesTaken((MessageBuffer)) > strlen(message)))
 * { */
/*         xMessageBufferSend(MessageBuffer, message, strlen(message), 0); */
/*         xSemaphoreGive(xSemaphoreMessageBuffer); */
/*       } */
/*       counter = 0; */
/*     } */

/*     // TODO: Task Schedule */
/*     const TickType_t X_DELAY = TASK_PERIOD / portTICK_PERIOD_MS; */
/*     vTaskDelay(X_DELAY); // one tick delay (15ms) */
/*     counter++; */
/*   } */
/* } */

void task_serial(void *pVParameters) // This is a task.
{
  (void)pVParameters;
  const uint8_t TASK_PERIOD = 200;
  char received_data[MESSAGE_SIZE];
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

    // Debug
    /* Serial.print("(Serial) isSerialSemaphoreTaken :"); */
    /* Serial.println(static_cast<int>(isSerialSemaphoreTaken)); */
    /* Serial.print("(Serial) isMessageBufferSemaphoreTaken :"); */
    /* Serial.println(static_cast<int>(isMessageBufferSemaphoreTaken)); */
    /* Serial.print("(Serial) isMessageBufferEmpty: "); */
    /* Serial.println(static_cast<int>(isMessageBufferEmpty)); */

    if (isSerialSemaphoreTaken) {
      if (isMessageBufferSemaphoreTaken) {
        if (!isMessageBufferEmpty) {
          /* Serial.println("Sesso."); */
          /* Serial.print("received_bytes: "); */
          received_bytes = xMessageBufferReceive(xMessageBuffer, received_data,
                                                 sizeof(received_data), 0);
          for (size_t ii = 0; ii < received_bytes -1; ii++) {
            Serial.print((char)received_data[ii]);
          }
          Serial.print("\n");
          xMessageBufferReset(xMessageBuffer);
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

/* void set_pid_gains(float *pKp, float *pKi, float *pKd) { */

/*   get_serial_message(&message); */

/*   case 'P': */
/*     *pKp = message.data.gain_value; */
/*     break; */
/*   case 'I': */
/*     *pKi = message.data.gain_value; */
/*     break; */
/*   case 'D': */
/*     *pKd = message.data.gain_value; */
/*     break; */
/*   default: */
/*     *pKp = 0.0; */
/*     *pKi = 0.0; */
/*     *pKd = 0.0; */
/*   } */
/* } */

/* void get_serial_message(union Message *pMessage) { */

/*   if (Serial.available() > 0) { */
/*     // gain_type */
/*     pMessage->data.gain_type = Serial.read(); */
/*     // gain value */
/*     if (Serial.available() == 1) { */
/*       pMessage->data.gain_value = (float)Serial.parseInt(); */
/*     } else { */
/*       pMessage->data.gain_value = Serial.parseFloat(); */
/*     } */
/*   } */
/* } */
