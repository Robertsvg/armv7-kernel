#include <arch/bsp/interrupts.h>
#include <arch/bsp/uart.h>
#include <lib/ringbuffer.h>
#include <stddef.h>
#include <stdint.h>

static constexpr uintptr_t UART_BUS_BASE     = 0x7e201000;
static constexpr uintptr_t PERIPHERAL_OFFSET = 0x3f000000;
static constexpr uint32_t  UART_RXFE	     = 1u << 4;
static constexpr uint32_t  UART_TXFF	     = 1u << 5;
static constexpr uint32_t  UART_FEN	     = 1u << 4;
static constexpr uint32_t  UART_RX_IRQ	     = 1u << 4;
static constexpr uint32_t  UART_RT_IRQ	     = 1u << 6;
static constexpr uint32_t  UART_CLEAR_ALL    = 0x7ff;

struct pl011 {
	uint32_t dr, rsr_ecr, unused[4], fr;
	uint32_t reserved, ilpr, ibrd, fbrd, lcr_h, cr, ifls, imsc, ris, mis, icr;
};

static_assert(offsetof(struct pl011, fr) == 0x18);
static_assert(offsetof(struct pl011, imsc) == 0x38);
static_assert(offsetof(struct pl011, icr) == 0x44);
static volatile struct pl011 *const uart = (struct pl011 *)(UART_BUS_BASE - PERIPHERAL_OFFSET);
create_ringbuffer(receive_buffer, 128);

void uart_init(void)
{
	uart->imsc = 0;
	uart->icr  = UART_CLEAR_ALL;
	/* Ohne Hardware-FIFO loest schon ein einzelnes Zeichen einen RX-Interrupt aus. */
	uart->lcr_h &= ~UART_FEN;
	uart->imsc = UART_RX_IRQ | UART_RT_IRQ;
	asm volatile("dsb sy" ::: "memory");
	interrupt_enable_uart();
}

void uart_handle_irq(void)
{
	uart->icr = UART_RX_IRQ | UART_RT_IRQ;
	while (!(uart->fr & UART_RXFE)) {
		buff_putc(receive_buffer, (char)(unsigned char)uart->dr);
	}
	asm volatile("dsb sy" ::: "memory");
}

char uart_getc(void)
{
	return buff_getc(receive_buffer);
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
