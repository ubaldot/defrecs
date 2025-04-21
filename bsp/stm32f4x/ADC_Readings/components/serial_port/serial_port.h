#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "application_setup.h"

void serial_port_init(void);
void serial_port_write_step(enum WhoIsCalling caller);
void serial_port_read_step(enum WhoIsCalling caller);
void subscribe_serial_port_rx_msg(char *);
#endif /* SERIAL_PORT_H */
