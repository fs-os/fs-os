
#ifndef KERNEL_EXCEPTIONS_H_
#define KERNEL_EXCEPTIONS_H_ 1

/**
 * @brief Exceptions header
 *
 * Concepts:
 *   - IRQ: Hardware interrupt (Interrupt request)
 *   - ISR: Interrupt Service Routine (Interrupt handler)
 *   - PIT: Programable Interrupt Timer
 *
 * For more information see the fs-os wiki.
 *
 * @file
 */

/**
 * @brief Disables interrupts and panics with the specified exception.
 * @details Defined in src/kernel/exceptions.c
 * @param code Exception code (Index in the IDT)
 * @param eip Value of EIP register pushed by the CPU before calling the ISR
 * (i.e. address where the exception occurred)
 *
 * @todo Also push error code to the stack inside EXC_WRAPPER_ERR (See idt.asm)
 */
void handle_exception(int exc, void* eip);

/**
 * @name Default exception handlers
 * @brief Call the exception handler with the specified IRQ (number function
 * name).
 * @details Used as ISR offsets for the idt. Defined in
 * src/kernel/exceptions.asm
 * @{ */
void exc_0(void);
void exc_debug(void);
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
/**  @} */

/**
 * @brief Assembly wrapper for the PIT IRQ.
 * @details Defined in src/kernel/idt.asm
 */
void irq_pit(void);

/**
 * @brief Assembly wrapper for the keyboard IRQ.
 * @details Defined in src/kernel/idt.asm
 */
void irq_kb(void);

/**
 * @brief Generic ISR for ignoring all unhandled IRQs from master PIC.
 * @details Defined in src/kernel/idt.asm
 */
void irq_default_master(void);

/**
 * @brief Generic ISR for ignoring all unhandled IRQs from slave PIC.
 * @details Defined in src/kernel/idt.asm
 */
void irq_default_slave(void);

#endif /* KERNEL_EXCEPTIONS_H_ */
