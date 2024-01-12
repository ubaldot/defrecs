#ifndef LM35_MAIN_H
#define LM35_MAIN_H

#include <Arduino.h>
//
void tempsens_init(void);
void tempsens_main(void);
void geto_tempsens_value(float *);

#endif /* LM35_MAIN_H */
