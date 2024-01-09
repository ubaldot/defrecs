
#include "blink_main.h"
#include "tasks_setup.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

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
const size_t MESSAGE_MAX_SIZE = 20;
const size_t QUEUE_MAX_LENGTH = 5;
QueueHandle_t xSerialQueue;

// Task functions prototypes
TaskHandle_t xTaskHandleBlink;
void task_blink(void *pVParameters);
void task_controller(void *pVParameters);
TaskHandle_t xTaskHandleSerial;
void task_serial(void *pVParameters);

// Aux functions
void set_pid_gains(float * /*Kp*/, float * /*Ki*/, float * /*Kd*/);
void get_serial_message(union Message * /*message*/);

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // SERIAL: initialize serial communication at 9600 bits per second:
  const uint16_t BAUDRATE = 9600;
  Serial.begin(BAUDRATE);

  // TODO: needed?
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO,
  }

  // Create Serial queue
  xSerialQueue = xQueueCreate(QUEUE_MAX_LENGTH, MESSAGE_MAX_SIZE);

  // Task: Blink
  BaseType_t xReturnedBlink;
  xReturnedBlink = xTaskCreate(task_blink, NAME_BLINK, STACK_SIZE_BLINK,
                               (void *)&TASK_PARAMS_BLINK, PRIORITY_BLINK,
                               &xTaskHandleBlink);
  // TODO:
  if (xReturnedBlink != pdPASS) {
    char msg[] = "I cannot instantiate the Blink task.";
    xQueueSend(xSerialQueue, msg, 1000 / portTICK_PERIOD_MS);
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
  BaseType_t xReturnedSerial;
  xReturnedSerial =
      xTaskCreate(task_serial, NAME_SERIAL_PORT, STACK_SIZE_SERIAL_PORT,
                  (void *)&TASK_PARAMS_SERIAL_PORT, PRIORITY_SERIAL_PORT,
                  &xTaskHandleSerial);

  if (xReturnedSerial != pdPASS) {
    Serial.println("I cannot instantiate the Serial task.");
  }
}
// Skip loop. OBS! IdleTask cannot be used!
void loop() {}

// Task implementation
void task_blink(void *pVParameters) // This is a task.
{
  TaskParamsBlink *params = (TaskParamsBlink *)pVParameters;


  while (true) {
    blink_main();

    uint8_t led_state;
    led_state = get_led_state();
    digitalWrite(LED_BUILTIN, led_state);

    static uint8_t counter = 0;
    const size_t MAX_COUNT = 2;
    uint8_t message;
    message = uxTaskGetStackHighWaterMark(xTaskHandleBlink);
    /* Serial.println(message); */
    /* char message[] = "wolf"; */
    if (counter == MAX_COUNT) {
      xQueueSend(xSerialQueue, &message, 1000 / portTICK_PERIOD_MS);
      counter = 0;
    } else {
      counter++;
    }

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
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
  char received_data[MESSAGE_MAX_SIZE];

  while (true) {
    // Conditions for writing on the serial
    if (uxQueueMessagesWaiting(xSerialQueue) != 0U) {
      BaseType_t isReceived;
      isReceived = xQueueReceive(xSerialQueue, &received_data,
                                 1000 / portTICK_PERIOD_MS);
      /* Serial.println(*received_data); */
      if (isReceived == pdPASS) {
        Serial.println(*received_data);
      }
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
