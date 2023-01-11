
#ifndef _KERNEL_MISC_ASM_H
#define _KERNEL_MISC_ASM_H

void asm_cli(void);
void asm_sti(void);

/* load_idt: loads the idt descriptor from the assembly using the lidt instruction */
void load_idt(void* idt_desc);

#endif /* _KERNEL_MISC_ASM_H */

