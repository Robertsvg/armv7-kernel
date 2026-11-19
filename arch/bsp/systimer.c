#include <arch/bsp/interrupts.h>
#include <arch/bsp/systimer.h>
#include <arch/bsp/uart.h>
#include <kernel/config.h>
#include <stddef.h>
#include <stdint.h>

static constexpr uintptr_t    SYSTIMER_BASE = 0x3f003000;
static constexpr unsigned int TIMER_MATCH_1 = 1u << 1;
static constexpr unsigned int TIMER_CHANNEL = 1;

struct system_timer {
	uint32_t cs, clo, chi;
	uint32_t compare[4];
};

static_assert(offsetof(struct system_timer, compare[TIMER_CHANNEL]) == 0x10);
static volatile struct system_timer *const timer = (struct system_timer *)SYSTIMER_BASE;
static unsigned int			   next_compare;

void systimer_init(void)
{
	timer->cs		      = TIMER_MATCH_1;
	next_compare		      = timer->clo + TIMER_INTERVAL;
	timer->compare[TIMER_CHANNEL] = next_compare;
	asm volatile("dsb sy" ::: "memory");
	interrupt_enable_timer();
}

void systimer_handle_irq(void)
{
	if (!(timer->cs & TIMER_MATCH_1)) {
		return;
	}
	timer->cs = TIMER_MATCH_1;
	do {
		next_compare += TIMER_INTERVAL;
	} while ((int32_t)(next_compare - timer->clo) <= 0);
	timer->compare[TIMER_CHANNEL] = next_compare;
	asm volatile("dsb sy" ::: "memory");
	uart_puts("!\n");
}
