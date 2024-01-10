#include "tasks_setup.h"
#include "blink_main.h"
#include "pinout_functions_arduino.h"
#include "serial_port.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// TODO: adjust these two here
TaskHandle_t xTaskHandle_200ms;
TaskHandle_t xTaskHandle_1000ms;

static void components_init(void);
static void task_200ms(void * /*pVParameters*/);
static void task_1000ms(void * /*pVParameters*/);

static void components_init() {
  // List all the components used. Initializes queues, mutex, etc.
  serial_port_init();
  blink_init(); // Initialize initial condition of the component, mutex,
}

void tasks_setup() {
  // Initialize all the included components.
  components_init();

  // From now on it is all the same. Copied and pasted.
  // Create task
  BaseType_t xReturned1000ms;
  xReturned1000ms = xTaskCreate(task_1000ms, NAME_1000MS, STACK_SIZE_1000MS,
                                (void **)&TASK_PARAMS_1000MS, PRIORITY_1000MS,
                                &xTaskHandle_1000ms);
  /* if (xReturned1000ms != pdPASS) { */
  /*   const char msg[] = "I cannot instantiate the 1000ms task."; */
  /*   serial_port_send(msg); */
  /* } */

  // Create task
  BaseType_t xReturned200ms;
  xReturned200ms = xTaskCreate(task_200ms, NAME_200MS, STACK_SIZE_200MS,
                               (void *)&TASK_PARAMS_200MS, PRIORITY_200MS,
                               &xTaskHandle_200ms);
  /* if (xReturned200ms != pdPASS) { */
  /*   // To do: change! */
  /*   const char msg[] = "I cannot instantiate the 200ms task."; */
  /*   // TODO */
  /*   /1* serial_port_send(msg); *1/ */
  /* } */
}

static void task_1000ms(void *pVParameters) // This is a task.
{
  TaskParamsBlink *params = (TaskParamsBlink *)pVParameters;

  // Init all the components running in this task.
  while (true) {
    blink_main(); // This only produces signals

    // TODO
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

static void task_200ms(void *pVParameters) // This is a task.
{
  TaskParamsSerialPort *params = (TaskParamsSerialPort *)pVParameters;

  while (true) {
    serial_port_main();

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}
