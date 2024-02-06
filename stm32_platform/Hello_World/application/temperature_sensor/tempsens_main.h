#ifndef LM35_MAIN_H
#define LM35_MAIN_H

//
void tempsens_init(void);
void tempsens_step(void);
void subscribe_tempsens_value(float *);

#endif /* LM35_MAIN_H */
