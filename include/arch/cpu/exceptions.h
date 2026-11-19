#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

enum exception_type {
	EXCEPTION_UNDEFINED,
	EXCEPTION_SVC,
	EXCEPTION_PREFETCH_ABORT,
	EXCEPTION_DATA_ABORT,
	EXCEPTION_IRQ,
};

struct exception_frame {
	unsigned int cpsr;
	unsigned int spsr;
	unsigned int r[13];
	unsigned int return_address;
};

struct mode_regs {
	unsigned int user_lr, user_sp;
	unsigned int irq_lr, irq_sp, irq_spsr;
	unsigned int abort_lr, abort_sp, abort_spsr;
	unsigned int undefined_lr, undefined_sp, undefined_spsr;
	unsigned int supervisor_lr, supervisor_sp, supervisor_spsr;
};

extern bool irq_debug;

void exception_handler(const struct exception_frame *frame, enum exception_type type);
void read_mode_specific_registers(struct mode_regs *regs);
void do_data_abort(void);
void do_prefetch_abort(void);
void do_supervisor_call(void);
void do_undefined_inst(void);

#endif
