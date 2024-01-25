#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H
#include <Arduino.h>

const size_t MESSAGE_SIZE_MAX = 100;

void serial_port_init(void);
void serial_port_main(void);
#endif /* SERIAL_PORT_H */
