#include <arch/bsp/interrupts.h>
#include <stddef.h>
#include <stdint.h>

static constexpr uintptr_t    IRQ_BASE	  = 0x3f00b200;
static constexpr uintptr_t    GPU_ROUTING = 0x4000000c;
static constexpr unsigned int TIMER_IRQ	  = 1u << 1;
static constexpr unsigned int UART_IRQ	  = 1u << (57 - 32);

struct interrupt_controller {
	uint32_t basic_pending, pending1, pending2, fiq_control;
	uint32_t enable1, enable2, enable_basic;
	uint32_t disable1, disable2, disable_basic;
};

static_assert(offsetof(struct interrupt_controller, enable1) == 0x10);
static_assert(offsetof(struct interrupt_controller, disable1) == 0x1c);

static volatile struct interrupt_controller *const controller =
	(struct interrupt_controller *)IRQ_BASE;

void interrupts_init(void)
{
	controller->disable1		  = UINT32_MAX;
	controller->disable2		  = UINT32_MAX;
	controller->disable_basic	  = UINT32_MAX;
	controller->fiq_control		  = 0;
	*(volatile uint32_t *)GPU_ROUTING = 0;
	asm volatile("dsb sy" ::: "memory");
}

void interrupt_enable_uart(void)
{
	controller->enable2 = UART_IRQ;
	asm volatile("dsb sy" ::: "memory");
}

void interrupt_enable_timer(void)
{
	controller->enable1 = TIMER_IRQ;
	asm volatile("dsb sy" ::: "memory");
}

bool interrupt_uart_pending(void)
{
	return (controller->pending2 & UART_IRQ) != 0;
}

bool interrupt_timer_pending(void)
{
	return (controller->pending1 & TIMER_IRQ) != 0;
}
