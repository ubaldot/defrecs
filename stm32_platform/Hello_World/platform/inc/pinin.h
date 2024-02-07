#ifndef IN_FUNCTIONS_H
#define IN_FUNCTIONS_H

#include <stdint.h>
#include "application_setup.h"
uint16_t pinin_temp_sens(void);
uint16_t pinin_pv(void);
uint16_t pinin_pv1(void);
void pinin_usart(uint8_t *, enum HAL_Function_Mode);

#endif /* IN_FUNCTIONS_H */
