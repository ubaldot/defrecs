#ifndef DEBUG_H
#define DEBUG_H

//
#include "application_setup.h"
#include <stddef.h>

void debug_init(void);
void debug_step(enum WhoIsCalling);
void subscribe_debug_task_1000ms_watermark(size_t *);
void subscribe_debug_task_200ms_watermark(size_t *);

#endif /* DEBUG_H */
