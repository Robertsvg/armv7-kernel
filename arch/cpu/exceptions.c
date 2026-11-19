#include <arch/bsp/interrupts.h>
#include <arch/bsp/systimer.h>
#include <arch/bsp/uart.h>
#include <arch/cpu/exceptions.h>
#include <arch/cpu/powerdown.h>
#include <arch/cpu/psr.h>
#include <kernel/config.h>
#include <lib/kprintf.h>
#include <stddef.h>

static constexpr unsigned int ARM_INSTRUCTION_SIZE = 4;
static constexpr unsigned int FSR_STATUS_HIGH_BIT  = 10;
static constexpr unsigned int FSR_STATUS_LOW_MASK  = 0xf;

static_assert(sizeof(struct exception_frame) == 64);
static_assert(offsetof(struct exception_frame, spsr) == 4);
static_assert(offsetof(struct exception_frame, r) == 8);
static_assert(offsetof(struct exception_frame, return_address) == 60);
static_assert(sizeof(struct mode_regs) == 56);

bool irq_debug = false;

static void print_psr(unsigned int psr)
{
	kprintf("%c%c%c%c %c %c%c%c", psr & PSR_N ? 'N' : '_', psr & PSR_Z ? 'Z' : '_',
		psr & PSR_C ? 'C' : '_', psr & PSR_V ? 'V' : '_', psr & PSR_E ? 'E' : '_',
		psr & PSR_I ? 'I' : '_', psr & PSR_F ? 'F' : '_', psr & PSR_T ? 'T' : '_');
	const char *name;
	switch (psr & PSR_MODE_MASK) {
	case PSR_USR:
		name = "      User";
		break;
	case PSR_SYS:
		name = "    System";
		break;
	case PSR_SVC:
		name = "Supervisor";
		break;
	case PSR_ABT:
		name = "     Abort";
		break;
	case PSR_IRQ:
		name = "       IRQ";
		break;
	case PSR_UND:
		name = " Undefined";
		break;
	default:
		name = "   Invalid";
		break;
	}
	kprintf(" %s 0x%08x", name, psr);
}

static const char *get_fsr_description(unsigned int fsr)
{
	static const char *const descriptions[] = {
		[0b00000] = "No function, reset value",
		[0b00001] = "Alignment fault",
		[0b00010] = "Debug event fault",
		[0b00011] = "Access Flag fault on Section",
		[0b00100] = "Cache maintenance operation fault",
		[0b00101] = "Translation fault on Section",
		[0b00110] = "Access Flag fault on Page",
		[0b00111] = "Translation fault on Page",
		[0b01000] = "Precise External Abort",
		[0b01001] = "Domain fault on Section",
		[0b01011] = "Domain fault on Page",
		[0b01100] = "External abort on Section",
		[0b01101] = "Permission fault on Section",
		[0b01110] = "External abort on Page",
		[0b01111] = "Permission fault on Page",
		[0b10000] = "TLB conflict abort",
		[0b10100] = "Implementation definedf fault",
		[0b10110] = "External Abort",
		[0b11000] = "Asynchronous parity error on memory access",
		[0b11001] = "Synchronous parity error on memory access",
		[0b11010] = "Implementation defined fault",
		[0b11100] = "Synchronous parity error on translation table walk on section",
		[0b11110] = "Synchronous parity error on translation table walk on page",
	};
	unsigned int status = (fsr & FSR_STATUS_LOW_MASK) | ((fsr >> FSR_STATUS_HIGH_BIT) & 1u)
								    << 4;
	if (status >= sizeof(descriptions) / sizeof(descriptions[0]) ||
	    descriptions[status] == NULL) {
		return "Invalid fault status register value";
	}
	return descriptions[status];
}

static void print_exception_infos(const struct exception_frame *frame, enum exception_type type)
{
	static const char *const names[] = {
		"Undefined Instruction",
		"Supervisor Call",
		"Prefetch Abort",
		"Data Abort",
		"Interrupt",
	};
	unsigned int address = frame->return_address;
	if (type == EXCEPTION_UNDEFINED || type == EXCEPTION_SVC) {
		address -= ARM_INSTRUCTION_SIZE;
	}
	kprintf("\n############ EXCEPTION ############\n");
	kprintf("%s an Adresse: 0x%08x\n", names[type], address);
	if (type == EXCEPTION_DATA_ABORT) {
		unsigned int dfsr, dfar;
		asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr));
		asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));
		kprintf("Data Fault Status Register: 0x%08x -> %s\n", dfsr,
			get_fsr_description(dfsr));
		kprintf("Data Fault Adress Register: 0x%08x\n", dfar);
	}
	if (type == EXCEPTION_PREFETCH_ABORT) {
		unsigned int ifsr, ifar;
		asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r"(ifsr));
		asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r"(ifar));
		kprintf("Instruction Fault Status Register: 0x%08x -> %s\n", ifsr,
			get_fsr_description(ifsr));
		kprintf("Instruction Fault Adress Register: 0x%08x\n", ifar);
	}

	const unsigned int *r = frame->r;
	kprintf("\n>> Registerschnappschuss <<\n");
	kprintf("R0: 0x%08x  R5: 0x%08x  R10: 0x%08x\n", r[0], r[5], r[10]);
	kprintf("R1: 0x%08x  R6: 0x%08x  R11: 0x%08x\n", r[1], r[6], r[11]);
	kprintf("R2: 0x%08x  R7: 0x%08x  R12: 0x%08x\n", r[2], r[7], r[12]);
	kprintf("R3: 0x%08x  R8: 0x%08x\n", r[3], r[8]);
	kprintf("R4: 0x%08x  R9: 0x%08x\n", r[4], r[9]);

	struct mode_regs modes;
	read_mode_specific_registers(&modes);
	kprintf("\n>> Modusspezifische Register <<\n");
	kprintf("User/System | LR: 0x%08x | SP: 0x%08x | CPSR: ", modes.user_lr, modes.user_sp);
	print_psr(frame->cpsr);
	kprintf("\nIRQ         | LR: 0x%08x | SP: 0x%08x | SPSR: ", modes.irq_lr, modes.irq_sp);
	print_psr(modes.irq_spsr);
	kprintf("\nAbort       | LR: 0x%08x | SP: 0x%08x | SPSR: ", modes.abort_lr, modes.abort_sp);
	print_psr(modes.abort_spsr);
	kprintf("\nUndefined   | LR: 0x%08x | SP: 0x%08x | SPSR: ", modes.undefined_lr,
		modes.undefined_sp);
	print_psr(modes.undefined_spsr);
	kprintf("\nSupervisor  | LR: 0x%08x | SP: 0x%08x | SPSR: ", modes.supervisor_lr,
		modes.supervisor_sp);
	print_psr(modes.supervisor_spsr);
	kprintf("\n");
}

void exception_handler(const struct exception_frame *frame, enum exception_type type)
{
	asm volatile("dsb sy" ::: "memory");
	if (type == EXCEPTION_IRQ) {
		if (interrupt_uart_pending()) {
			uart_handle_irq();
		}
		if (interrupt_timer_pending()) {
			systimer_handle_irq();
		}
		if (irq_debug) {
			print_exception_infos(frame, type);
		}
		return;
	}
	print_exception_infos(frame, type);
	uart_putc('\4');
	asm volatile("cpsid if" ::: "memory");
#ifdef BUILD_FOR_QEMU
	powerdown();
#else
	while (true) {
		asm volatile("wfi");
	}
#endif
}
