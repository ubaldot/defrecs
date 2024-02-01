#ifndef DEBUG_MAIN_H
#define DEBUG_MAIN_H

//
#include <stddef.h>
void debug_init(void);
void debug_main(void);
void geto_debug_task_1000ms_watermark(size_t *);
void geto_debug_task_200ms_watermark(size_t *);

#endif /* DEBUG_MAIN_H */
