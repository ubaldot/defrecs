#include "application_setup.h"
#include "blink/blink.h"
#include "gpio.h"

void digital_out_init(void){};

void digital_out_step(enum WhoIsCalling caller) {
  (void)caller;
  uint8_t led_state;
  subscribe_blink_led_state(&led_state);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, led_state);
}
