#ifndef PININ_H
#define PININ_H

#include "application_setup.h"
#include <stddef.h>
#include <stdint.h>


void pinin_init(void);

void pinin_pv(float *);
void pinin_usart(uint8_t *, size_t len);
float pinin_temp_sens(void);

#endif /* PININ_H */
