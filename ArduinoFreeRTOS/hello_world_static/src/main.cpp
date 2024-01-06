/*
 * Example of FreeRTOS configASSERT macro
 * https://www.freertos.org/a00110.html#configASSERT
 */

#include "Arduino.h"
#include "HardwareSerial.h"
#include <Arduino_FreeRTOS.h>
#include "FreeRTOSConfig.h"

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

// Task: Blink
void task_blink(void *pVParameters);
// TCB is the part of memory that keeps the task state
static StaticTask_t xTCBBlink;
static StackType_t xStackSizeBlink[128];

// Task: AnalogRead
void task_analog_read(void *pVParameters);
static StaticTask_t xTCBAnalogRead;
static StackType_t xStackSizeAnalogRead[128];

// Aux functions
void set_pid_gains(float * /*Kp*/, float * /*Ki*/, float * /*Kd*/);
void get_serial_message(union Message * /*message*/);

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  const unsigned int BAUDRATE = 9600;
  Serial.begin(BAUDRATE);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO,
      // MICRO, YUN, and other 32u4 based boards.
  }

  // Now set up two tasks to run independently.
  // Blink
  const int STACK_DEPTH_BLINK = 128;
  const int PRIORITY_BLINK = 2;
  TaskHandle_t xTaskHandleBlink = NULL;
  xTaskHandleBlink = xTaskCreateStatic(task_blink, "Blink", STACK_DEPTH_BLINK, NULL, PRIORITY_BLINK,xStackSizeBlink, &xTCBBlink);

  // AnalogRead
  const int STACK_DEPTH_ANALOG_READ = 128;
  const int PRIORITY_ANALOG_READ = 2;
  TaskHandle_t xTaskHandleAnalogRead = NULL;
  xTaskHandleAnalogRead = xTaskCreateStatic(task_analog_read, "AnalogRead", STACK_DEPTH_ANALOG_READ, NULL,
              PRIORITY_ANALOG_READ,xStackSizeAnalogRead, &xTCBAnalogRead);
}

void loop() {}

void task_blink(void *pVParameters) // This is a task.
{
  (void)pVParameters;


  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    static bool led_state = false;
    if (!led_state) {
      digitalWrite(LED_BUILTIN,
                   HIGH); // turn the LED on (HIGH is the voltage level)
      led_state = true;
    } else {
      digitalWrite(LED_BUILTIN,
                   LOW); // turn the LED off by making the voltage LOW
      led_state = false;
    }

    // Task Schedule
    const int TASK_PERIOD = 1000;
    const TickType_t X_DELAY = TASK_PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms) in between reads for stability
  }
}

void task_analog_read(void *pVParameters) // This is a task.
{
  (void)pVParameters;

  // PWN on pin 5
  const int FAN_PIN = 5;
  pinMode(FAN_PIN, OUTPUT);

  for (;;) {
    // read the input on analog pin 0:
    int sensor_value;
    sensor_value = analogRead(A0);
    // analogRead return a 0-1024, whereas analogWrite is 0-255
    // Controller has gain 1
    static float kp;
    static float ki;
    static float kd;
    set_pid_gains(&kp, &ki, &kd);
    int pwm_value;
    pwm_value = sensor_value >> 2;
    analogWrite(FAN_PIN, pwm_value);
    // print out the value you read:
    /* Serial.print(sensorValue); */
    /* Serial.print(","); */
    /* Serial.print(pwm_value); */
    /* Serial.print(","); */
    /* Serial.print(Kp); */
    /* Serial.print("\n\r"); */

    // Task Schedule
    const int TASK_PERIOD = 200;
    const TickType_t X_DELAY = TASK_PERIOD / portTICK_PERIOD_MS;
    vTaskDelay(X_DELAY); // one tick delay (15ms) in between reads for stability
  }
}

void set_pid_gains(float *pKp, float *pKi, float *pKd) {


  get_serial_message(&message);

  switch (message.data.gain_type) {
  case 'P':
    *pKp = message.data.gain_value;
    break;
  case 'I':
    *pKi = message.data.gain_value;
    break;
  case 'D':
    *pKd = message.data.gain_value;
    break;
  default:
    *pKp = 0.0;
    *pKi = 0.0;
    *pKd = 0.0;
  }
  /* Serial.print(*Kp); */
  /* Serial.print(","); */
  /* Serial.print(*Ki); */
  /* Serial.print("\n\r"); */
}

void get_serial_message(union Message *pMessage) {

  if (Serial.available() > 0) {
    // gain_type
    pMessage->data.gain_type = Serial.read();
    // gain value
    if (Serial.available() == 1) {
      pMessage->data.gain_value = (float)Serial.parseInt();
    } else {
      pMessage->data.gain_value = Serial.parseFloat();
    }
  }
}
