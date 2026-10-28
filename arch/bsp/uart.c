#include <arch/bsp/uart.h>
#include <stddef.h>
#include <stdint.h>

static constexpr uintptr_t UART_BUS_BASE     = 0x7e201000;
static constexpr uintptr_t PERIPHERAL_OFFSET = 0x3f000000;
static constexpr uint32_t  UART_RXFE	     = 1u << 4;
static constexpr uint32_t  UART_TXFF	     = 1u << 5;

struct pl011 {
	uint32_t dr;
	uint32_t rsr_ecr;
	uint32_t unused[4];
	uint32_t fr;
};

static_assert(offsetof(struct pl011, dr) == 0x00);
static_assert(offsetof(struct pl011, fr) == 0x18);

static volatile struct pl011 *const uart = (struct pl011 *)(UART_BUS_BASE - PERIPHERAL_OFFSET);

char uart_getc(void)
{
	while (uart->fr & UART_RXFE) {
	}
	return (char)(unsigned char)uart->dr;
}

void uart_putc(char c)
{
	while (uart->fr & UART_TXFF) {
	}
	uart->dr = (unsigned char)c;
}

void uart_puts(const char *text)
{
	while (*text != '\0') {
		uart_putc(*text++);
	}
}

void uart_loopback [[noreturn]] (void)
{
	while (true) {
		uart_putc(uart_getc());
	}
}
