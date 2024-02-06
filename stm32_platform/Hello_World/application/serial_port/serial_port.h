#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "application_setup.h"


#define TX_MSG_LENGTH_MAX 100
#define RX_MSG_LENGTH_MAX 6

void serial_port_init(void);
void serial_port_step(enum WhoIsCalling);
#endif /* SERIAL_PORT_H */
