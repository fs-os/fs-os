
#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#include <stdint.h>

/* pit_init: initialize the programmable interval timer with the specified frequency
 * in interrupts per seccond (freq 1000 would mean 1000 ticks in 1 sec) */
void pit_init(uint32_t freq);

/* pit_dec: decrease the current tick count. Called from the PIT interrupt, on:
 * src/kernel/idt_asm.asm */
void pit_dec(void);

/* pit_set_ticks: sets the current PIT tick count. Called from the sleep functions
 * for setting the ammount of time we want to wait (src/libk/time.c) */
void pit_set_ticks(uint64_t num);

/* pit_get_ticks: returns the current PIT tick count. Called from the sleep functions
 * for checking if the ticks we set have passed or not (reached 0) */
uint64_t pit_get_ticks(void);

#endif /* _KERNEL_PIT_H */
