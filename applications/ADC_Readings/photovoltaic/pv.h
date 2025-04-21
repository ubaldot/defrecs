#ifndef PHOTOVOLTAIC_H
#define PHOTOVOLTAIC_H

#include "application_setup.h"
//
void pv_init(void);
void pv_step(enum WhoIsCalling);
void subscribe_pv_voltage(float *);

#endif /* PHOTOVOLTAIC_H */
