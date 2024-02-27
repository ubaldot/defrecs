#ifndef USART2_H
#define USART2_H

#include "application_setup.h"

#define MSG_LENGTH_MAX 100

void usart2_init(void);
void usart2_step(enum WhoIsCalling);
#endif /* USART2_H */
