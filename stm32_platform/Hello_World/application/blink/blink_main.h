#ifndef BLINK_MAIN_H
#define BLINK_MAIN_H

//
#include "application_setup.h"
#include <stdint.h>

void blink_init(void);
void blink_main(enum WhoIsCalling);
void subscribe_blink_led_state(uint8_t *);

#endif /* BLINK_MAIN_H */
