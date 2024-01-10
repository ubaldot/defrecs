#ifndef BLINK_MAIN_H
#define BLINK_MAIN_H

#include <Arduino.h>
//
void blink_init(void);
void blink_main(void);
void get_blink_led_state(uint8_t *);

#endif /* BLINK_MAIN_H */
