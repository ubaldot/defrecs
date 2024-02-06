#ifndef PHOTOVOLTAIC_MAIN_H
#define PHOTOVOLTAIC_MAIN_H

#include "application_setup.h"
//
void pv_init(void);
void pv_main(enum WhoIsCalling);
void geto_pv_voltage(float *);

#endif /* PHOTOVOLTAIC_MAIN_H */
