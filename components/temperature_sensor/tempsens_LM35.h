#ifndef LM35_H
#define LM35_H

//
void tempsens_init(void);
void tempsens_step(void);
void subscribe_tempsens_value(float *);

#endif /* LM35_H */
