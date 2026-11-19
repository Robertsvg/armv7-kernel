#include <arch/cpu/psr.h>

.syntax unified
.arm
.text
.balign 32
.global exception_vectors
exception_vectors:
	b .
	b undefined_entry
	b svc_entry
	b prefetch_abort_entry
	b data_abort_entry
	b .
	b irq_entry
	b .

/* 14 Register und zwei Statusregister ergeben einen 8-Byte-ausgerichteten Frame. */
.macro trampoline name, number, lr_offset
\name:
	sub lr, lr, #\lr_offset
	stmdb sp!, {r0-r12, lr}
	mrs r0, cpsr
	mrs r1, spsr
	stmdb sp!, {r0, r1}
	mov r0, sp
	mov r1, #\number
	bl exception_handler
	ldr r0, [sp, #4]
	msr spsr_cxsf, r0
	add sp, sp, #8
	ldmia sp!, {r0-r12, pc}^
.endm

trampoline undefined_entry,      0, 0
trampoline svc_entry,            1, 0
trampoline prefetch_abort_entry, 2, 4
trampoline data_abort_entry,     3, 8
trampoline irq_entry,            4, 4

/* Auf die Register des aktuellen Modus darf kein banked MRS zugreifen. */
.macro read_mode mode, bank_lr, bank_sp, bank_spsr
	cmp r12, #\mode
	bne .Lbanked\@
	mov r1, lr
	mov r2, sp
	mrs r3, spsr
	b .Lstore\@
.Lbanked\@:
	mrs r1, \bank_lr
	mrs r2, \bank_sp
	mrs r3, \bank_spsr
.Lstore\@:
	stmia r0!, {r1-r3}
.endm

.global read_mode_specific_registers
.type read_mode_specific_registers, %function
read_mode_specific_registers:
	mrs r12, cpsr
	and r12, r12, #PSR_MODE_MASK
	mrs r1, lr_usr
	mrs r2, sp_usr
	stmia r0!, {r1, r2}
	read_mode PSR_IRQ, lr_irq, sp_irq, spsr_irq
	read_mode PSR_ABT, lr_abt, sp_abt, spsr_abt
	read_mode PSR_UND, lr_und, sp_und, spsr_und
	read_mode PSR_SVC, lr_svc, sp_svc, spsr_svc
	bx lr
.size read_mode_specific_registers, .-read_mode_specific_registers

.global do_data_abort
.type do_data_abort, %function
do_data_abort:
	mov r0, #1
	ldr r0, [r0]
	bx lr
.size do_data_abort, .-do_data_abort

.global do_prefetch_abort
.type do_prefetch_abort, %function
do_prefetch_abort:
	bkpt #0
	bx lr
.size do_prefetch_abort, .-do_prefetch_abort

.global do_supervisor_call
.type do_supervisor_call, %function
do_supervisor_call:
	svc #0
	bx lr
.size do_supervisor_call, .-do_supervisor_call

.global do_undefined_inst
.type do_undefined_inst, %function
do_undefined_inst:
	udf #0
	bx lr
.size do_undefined_inst, .-do_undefined_inst

.section .note.GNU-stack,"",%progbits
