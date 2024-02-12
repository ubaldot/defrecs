#ifndef PINOUT_H
#define PINOUT_H

#include <stddef.h>
#include <stdint.h>
void pinout_builtin_led(uint8_t);
void pinout_fan1(uint8_t);
void pinout_serial_port(const uint8_t *, size_t);

#endif /* PINOUT_H */
