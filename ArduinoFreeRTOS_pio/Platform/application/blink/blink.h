#ifndef BLINK_H
#define BLINK_H

//
#include "application_setup.h"
#include <stdint.h>

void blink_init(void);
void blink_step(enum WhoIsCalling);
void subscribe_blink_led_state(uint8_t *);

#endif /* BLINK_H */
