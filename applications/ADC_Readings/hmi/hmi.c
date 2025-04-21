//===-------------------- hmi.c ------------------------*- C -*-===//
// This component handle buttons, keyboard and read/write from the serial
// port.
//
// The received messages from the serial port are read byte by byte through
// interrupts. When a // terminator character is detected, then the content
// of rx_buffer shall be published into some appropriate signal.
//
// PREFIX: hmi_
// PUBLISHED SIGNALS: None.
//===----------------------------------------------------------------------===//
#include "hmi/hmi.h"
#include "application_setup.h"
#include "blink/blink.h"
#include "ftoa.h"
#include "photovoltaic/pv.h"
#include "tempsens_LM335/tempsens_LM335.h"
#include "hmi.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <task.h>


static char tx_buffer[MSG_LENGTH_MAX];
static char rx_buffer[MSG_LENGTH_MAX];
static size_t ii; // For counting the number of bytes received

static SemaphoreHandle_t mutex_tx_buffer; // This also protect ii
static SemaphoreHandle_t mutex_rx_buffer; // This also protect ii

// Publish
static void publish_hmi_tx_msg(const char *pMsg) {
  if (xSemaphoreTake(mutex_tx_buffer, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(&tx_buffer, pMsg, strlen(pMsg));
    xSemaphoreGive(mutex_tx_buffer);
  }
}

void subscribe_hmi_tx_msg(char *pMsg) {
  if (xSemaphoreTake(mutex_tx_buffer, 100 / portTICK_PERIOD_MS) ==
      pdTRUE) {
    memcpy(pMsg, &tx_buffer, strlen(tx_buffer) + 1);
    xSemaphoreGive(mutex_tx_buffer);
  }
}

void hmi_init() {
  mutex_tx_buffer = xSemaphoreCreateMutex();
  mutex_rx_buffer = xSemaphoreCreateMutex();
  ii = 0;
}

void hmi_step(enum WhoIsCalling caller) {
  // INPUTS
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);
  float pv_voltage;
  float tempsens_C;

  char msg[MSG_LENGTH_MAX];

  switch (caller) {
  case PERIODIC_TASK:
    subscribe_pv_voltage(&pv_voltage);
    subscribe_tempsens_value(&tempsens_C);

    char pv_voltage_str[5];
    char tempsens_C_str[5];
    (void)ftoa(pv_voltage, pv_voltage_str, 2);
    (void)ftoa(tempsens_C, tempsens_C_str, 2);
    (void)snprintf(msg, MSG_LENGTH_MAX,
                   "Photovoltaic reading: %s V\n Temperature: %s C\n",
                   pv_voltage_str, tempsens_C_str);
    publish_hmi_tx_msg(msg);
    break;
    /* What starts with IRQ are callbacks! */
  case IRQ_BUILTIN_BUTTON:

    (void)snprintf(msg, MSG_LENGTH_MAX, "Button pressed!\n");
    publish_hmi_tx_msg(msg);
    break;

  case IRQ_SERIAL_RX:
    if (xSemaphoreTake(mutex_rx_buffer, pdMS_TO_TICKS(5)) == pdTRUE) {
      if (rx_buffer[ii] == '\n' || ii > MSG_LENGTH_MAX) {
        /* publish_hmi_rx_message(msg, strlen(msg)); */
        memcpy(msg, rx_buffer, MSG_LENGTH_MAX - 1);
        msg[MSG_LENGTH_MAX - 1] = '\0';

        publish_hmi_tx_msg(msg);
        /* No needed because the serial port task is scheduled periodically anyway */
        /* serial_port_write_step(IRQ_SERIAL_RX) */

        /* Reinitialize all the variables used */
        ii = 0;
        memset(rx_buffer, '\0', MSG_LENGTH_MAX);
      } else {
        ii++;
      }
      /* subscribe_serial_port_rx_msg(&rx_buffer[ii]); */
      xSemaphoreGive(mutex_rx_buffer);
    }
    break;
  default:
    strncpy(msg, "Sto cazzo.\n", MSG_LENGTH_MAX - 1);
    msg[MSG_LENGTH_MAX - 1] = '\0';
    break;
  }
}
