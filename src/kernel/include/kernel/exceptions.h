
#ifndef _KERNEL_EXCEPTIONS_H
#define _KERNEL_EXCEPTIONS_H

/* handle_exception: disables interrupts and panics with the specified exception.
 * Defined in src/kernel/exceptions.c */
void handle_exception(int exc);

/* exc_X: call the exception handler with the specified IRQ. Used as ISR offsets for
 * the idt.
 * Defined in src/kernel/exceptions_asm.asm */
void exc_0(void);
void exc_1(void);
void exc_2(void);
void exc_3(void);
void exc_4(void);
void exc_5(void);
void exc_6(void);
void exc_7(void);
void exc_8(void);
void exc_10(void);
void exc_11(void);
void exc_12(void);
void exc_13(void);
void exc_14(void);
void exc_15(void);
void exc_16(void);
void exc_17(void);
void exc_18(void);
void exc_19(void);
void exc_20(void);
void exc_30(void);

/* Assembly wrappers for the PIT and keyboard IRQs.
 * Defined in src/kernel/idt_asm.asm */
void irq_pit(void);
void irq_kb(void);

/* Generic handlers for ignoring all IRQs we don't want from master and slave PIC.
 * Defined in src/kernel/idt_asm.asm */
void irq_default_master(void);
void irq_default_slave(void);

#endif /* _KERNEL_EXCEPTIONS_H */

