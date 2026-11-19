#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void interrupts_init(void);
void interrupt_enable_uart(void);
void interrupt_enable_timer(void);
bool interrupt_uart_pending(void);
bool interrupt_timer_pending(void);

#endif
