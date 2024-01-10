#ifndef BLINK_MAIN_H
#define BLINK_MAIN_H

#include <Arduino.h>
//
void blink_main_init(void);
void blink_main(void);

// signals
uint8_t get_blink_led_state(void);
#endif /* BLINK_MAIN_H */
