#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// Task Blink
const char NAME_BLINK[] = "Blink";
const size_t STACK_SIZE_BLINK = 90;
const uint8_t PRIORITY_BLINK = 2;
const struct TaskParamsBlink {
  TickType_t PERIOD = 1000;
} TASK_PARAMS_BLINK;
/* const struct TaskParams PARAMS_BLINK = {.PERIOD = 1000}; */

// Task Serial Port
const char NAME_SERIAL_PORT[] = "SerialPort";
const size_t STACK_SIZE_SERIAL_PORT = 315;
const uint8_t PRIORITY_SERIAL_PORT = 2;
const struct TaskParamsSerialPort {
  TickType_t PERIOD = 200;
} TASK_PARAMS_SERIAL_PORT;
