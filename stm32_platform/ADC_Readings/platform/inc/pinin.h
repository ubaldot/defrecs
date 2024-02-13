#ifndef PININ_H
#define PININ_H

#include "application_setup.h"
#include <stddef.h>
#include <stdint.h>


void pinin_init(void);

float pinin_temp_sens(void);

void pinin_pv(float *, enum HAL_Function_Mode);
void pinin_usart(uint8_t *, size_t len, enum HAL_Function_Mode);

#endif /* PININ_H */
