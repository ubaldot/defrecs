#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// Task 1000ms
const char NAME_1000MS[] = "Task_1000ms";
const size_t STACK_SIZE_1000MS = 128;
const uint8_t PRIORITY_1000MS = 2;
const struct TaskParamsBlink {
  TickType_t PERIOD = 1000;
} TASK_PARAMS_1000MS;
// Task functions prototypes
void task_1000ms(void *);

// Task 200ms
const char NAME_200MS[] = "Task_200ms";
const size_t STACK_SIZE_200MS = 256;
const uint8_t PRIORITY_200MS = 2;
const struct TaskParamsSerialPort {
  TickType_t PERIOD = 200;
} TASK_PARAMS_200MS;

void task_200ms(void *);
