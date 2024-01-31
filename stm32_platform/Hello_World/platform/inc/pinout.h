#ifndef OUT_FUNCTIONS_H
#define OUT_FUNCTIONS_H

#include <stdint.h>

void pinout_builtin_led(uint8_t);
void pinout_fan1(uint8_t);
void pinout_serial_port(const char *);

#endif /* OUT_FUNCTIONS_H */
