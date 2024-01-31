#include "FreeRTOS.h"
#include "blink/blink_main.h"
#include "debug/debug_main.h"
#include "photovoltaic/pv_main.h"
#include "serial_port/serial_port.h"
#include "temperature_sensor/tempsens_main.h"
#include <portmacro.h>
#include <task.h>

// Define parameters to be passed to tasks. See FreeRTOS guideline.
struct TaskParams {
  TickType_t PERIOD;
};

// Task 1000ms
TaskHandle_t xTaskHandle_1000ms;
const char NAME_1000MS[] = "Task_1000ms";
const size_t STACK_SIZE_1000MS = 256;
const uint8_t PRIORITY_1000MS = 2;
const struct TaskParams TASK_PARAMS_1000MS = {.PERIOD = 1000};

// Task 200ms
TaskHandle_t xTaskHandle_200ms;
const char NAME_200MS[] = "Task_200ms";
const size_t STACK_SIZE_200MS = 256;
const uint8_t PRIORITY_200MS = 2;
const struct TaskParams TASK_PARAMS_200MS = {.PERIOD = 200};

// Task function prototypes
static void components_init(void);
static void task_200ms(void * /*pVParameters*/);
static void task_1000ms(void * /*pVParameters*/);

static void components_init() {
  // List all the components used. Initializes queues, mutex, etc.
  blink_init();
  debug_init();
  serial_port_init();
  /* pv_init(); */
  /* tempsens_init(); */
}

void application_setup() {
  // Initialize all the components needed for this app.
  components_init();

  // Create tasks
  xTaskCreate(task_1000ms, NAME_1000MS, STACK_SIZE_1000MS,
              (void *)&TASK_PARAMS_1000MS, PRIORITY_1000MS,
              &xTaskHandle_1000ms);

  xTaskCreate(task_200ms, NAME_200MS, STACK_SIZE_200MS,
              (void *)&TASK_PARAMS_200MS, PRIORITY_200MS, &xTaskHandle_200ms);

  // Start scheduler
  // TODO: Add the idle task
  xPortStartScheduler();
}

static void task_1000ms(void *pVParameters) // This is a task.
{
  const struct TaskParams *params = (struct TaskParams *)pVParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    // Run activities
    blink_main();
    debug_main();
    serial_port_main();
    /* tempsens_main(); */

    // TODO Replace vTaskDelayUntil with xTaskDelayUntil
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(params->PERIOD));
  }
}

static void task_200ms(void *pVParameters) // This is a task.
{
  const struct TaskParams *params = (struct TaskParams *)pVParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    /* pv_main(); */

    // Task Schedule
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(params->PERIOD));
  }
}
