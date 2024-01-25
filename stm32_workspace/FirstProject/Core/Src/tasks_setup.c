#include "blink/blink_main.h"
#include "debug/debug_main.h"
#include "photovoltaic/pv_main.h"
#include "serial_port/serial_port.h"
#include "temperature_sensor/tempsens_main.h"
#include "FreeRTOS.h"
#include <task.h>
#include <portmacro.h>

// Define parameters to be passed to tasks. See FreeRTOS guideline.
struct TaskParams{
  TickType_t PERIOD;
};

// Task 1000ms
TaskHandle_t xTaskHandle_1000ms;
const char NAME_1000MS[] = "Task_1000ms";
const size_t STACK_SIZE_1000MS = 256;
const uint8_t PRIORITY_1000MS = 2;
const struct TaskParams TASK_PARAMS_1000MS = { .PERIOD = 1000};

// Task 200ms
TaskHandle_t xTaskHandle_200ms;
const char NAME_200MS[] = "Task_200ms";
const size_t STACK_SIZE_200MS = 256;
const uint8_t PRIORITY_200MS = 2;
const struct TaskParams TASK_PARAMS_200MS = { .PERIOD = 200};

// Task function prototypes
static void components_init(void);
static void task_200ms(void * /*pVParameters*/);
static void task_1000ms(void * /*pVParameters*/);

static void components_init() {
  // List all the components used. Initializes queues, mutex, etc.
  blink_init(); // Initialize initial condition of the component, mutex,
  debug_init(); // Initialize initial condition of the component, mutex,
  serial_port_init();
  pv_init();
  tempsens_init();
}

void tasks_setup() {
  // Initialize all the included components.
  components_init();

  // From now on it is all the same. Copied and pasted.
  // Create task
  BaseType_t xReturned1000ms;
  xReturned1000ms = xTaskCreate(task_1000ms, NAME_1000MS, STACK_SIZE_1000MS,
                                (void *)&TASK_PARAMS_1000MS, PRIORITY_1000MS,
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
  const struct TaskParams *params = (struct TaskParams *)pVParameters;

  while (1) {
    // Run activities
    blink_main();
    debug_main();
    tempsens_main();

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}

static void task_200ms(void *pVParameters) // This is a task.
{
  const struct TaskParams *params = (struct TaskParams *)pVParameters;

  while (1) {
    serial_port_main();
    pv_main();

    // Task Schedule
    const TickType_t X_DELAY = params->PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms)
  }
}