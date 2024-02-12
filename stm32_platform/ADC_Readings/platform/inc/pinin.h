#ifndef PININ_H
#define PININ_H

#include "application_setup.h"
#include <stddef.h>
#include <stdint.h>
float pinin_temp_sens(void);
float pinin_pv(void);
float pinin_pv1(void);
void pinin_usart(uint8_t *, size_t len, enum HAL_Function_Mode);

#endif /* PININ_H */
