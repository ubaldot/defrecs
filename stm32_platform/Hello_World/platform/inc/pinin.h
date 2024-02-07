#ifndef PININ_H
#define PININ_H

#include "application_setup.h"
#include <stddef.h>
#include <stdint.h>
uint16_t pinin_temp_sens(void);
uint16_t pinin_pv(void);
uint16_t pinin_pv1(void);
void pinin_usart(uint8_t *, size_t len, enum HAL_Function_Mode);

#endif /* PININ_H */
