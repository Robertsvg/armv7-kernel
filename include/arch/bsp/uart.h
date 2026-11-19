#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_handle_irq(void);
char uart_getc(void);
void uart_putc(char c);
void uart_puts(const char *text);
void uart_loopback [[noreturn]] (void);

#endif
