#ifndef INTERRUPTS_TO_TASKS_H
#define INTERRUPTS_TO_TASKS_H

#include <stdint.h>
void interrupts_to_tasks_init(void);

// Signals
void subscribe_irq_raw_rx_message(uint8_t *);
void subscribe_irq_builtin_button_message(uint8_t *);

#endif /* INTERRUPTS_TO_TASKS_H */
