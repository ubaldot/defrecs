#ifndef LM35_H
#define LM35_H

#include "application_setup.h"
//
void tempsens_init(void);
void tempsens_step(enum WhoIsCalling);
void subscribe_tempsens_value(float *);

#endif /* LM35_H */
