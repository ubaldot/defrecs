
#include "blink_main.h"
#include "hw_config.h"
#include "out_functions.h"
#include "serial_port.h"
#include "tasks_setup.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

TaskHandle_t xTaskHandle_1000ms;
TaskHandle_t xTaskHandle_200ms;

void setup() {

  // Hardware init
  GPIO_init();
  SerialPort_init();

  // Task: 1000ms
  // Initialize all the activities that will end up in this task
  blink_main_init();
  // Initialize the task itself.
  BaseType_t xReturned1000ms;
  xReturned1000ms = xTaskCreate(task_1000ms, NAME_1000MS, STACK_SIZE_1000MS,
                                (void *)&TASK_PARAMS_1000MS, PRIORITY_1000MS,
                                &xTaskHandle_1000ms);
  // TODO: THIS TEST SUCKS!!!!
  if (xReturned1000ms != pdPASS) {
    char msg[] = "I cannot instantiate the Blink task.";
    serial_port_send(msg);
  }

  // Task: 200ms
  serial_port_init();
  BaseType_t xReturned200ms;
  xReturned200ms = xTaskCreate(task_200ms, NAME_200MS, STACK_SIZE_200MS,
                               (void *)&TASK_PARAMS_200MS, PRIORITY_200MS,
                               &xTaskHandle_200ms);
  if (xReturned200ms != pdPASS) {
    Serial.println("I cannot instantiate the Serial task.");
  }
}

// Skip loop. OBS! IdleTask cannot be used!
void loop() {}

// Task implementation
void task_1000ms(void *pVParameters) // This is a task.
{
  TaskParamsBlink *params = (TaskParamsBlink *)pVParameters;

  // Init all the components running in this task.
  while (true) {
    blink_main(); // This only produces signals
    out_builtin_led();
    char msg[] = "testing...";
    serial_port_send(msg);

    // Debug
    /* static uint8_t counter = 0; */
    /* const size_t MAX_COUNT = 2; */
    /* uint8_t message; */
    /* message = uxTaskGetStackHighWaterMark(xTaskHandleBlink); */
    /* if (counter == MAX_COUNT) { */
    /*   xQueueSend(xSerialQueue, &message, 1000 / portTICK_PERIOD_MS); */
    /*   counter = 0; */
    /* } else { */
    /*   counter++; */
    /* } */

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}

void task_200ms(void *pVParameters) // This is a task.
{
  TaskParamsSerialPort *params = (TaskParamsSerialPort *)pVParameters;

  while (true) {
    serial_port_main();

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}
