#ifndef DEBUG_MAIN_H
#define DEBUG_MAIN_H

//
#include "application_setup.h"
#include <stddef.h>

void debug_init(void);
void debug_main(enum WhoIsCalling);
void geto_debug_task_1000ms_watermark(size_t *);
void geto_debug_task_200ms_watermark(size_t *);

#endif /* DEBUG_MAIN_H */
