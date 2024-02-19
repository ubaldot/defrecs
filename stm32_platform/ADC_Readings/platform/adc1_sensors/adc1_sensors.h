#ifndef ADC1_SENSORS_H
#define ADC1_SENSORS_H

#include "application_setup.h"

void adc1_sensors_init(void);
void adc1_sensors_step(enum WhoIsCalling);

void subscribe_adc1_pv_pin_voltage(float *);

#endif /* ADC1_SENSORS_H */
