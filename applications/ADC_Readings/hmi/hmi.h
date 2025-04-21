#ifndef HMI
#define HMI

#include "application_setup.h"

#define MSG_LENGTH_MAX 100

void hmi_init(void);
void hmi_step(enum WhoIsCalling);
void subscribe_hmi_tx_msg(char *);
#endif /* HMI */
