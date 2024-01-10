#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>


void serial_port_init(void);
char *serial_get_data(void);
void serial_port_main(void);
void serial_port_send(const char *);
#endif /* SERIAL_PORT_H */
