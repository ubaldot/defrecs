#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

const size_t MESSAGE_SIZE_MAX = 30;
const size_t QUEUE_LENGTH = 5;

void serial_port_init(void);
void serial_port_main(void);
void serial_port_send(const char *);
#endif /* SERIAL_PORT_H */
