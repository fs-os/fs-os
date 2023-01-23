
#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

/* kb_handler: actual C handler for the keyboard exceptions recieved from "irq_kb".
 * See src/kernel/idt_asm.asm */
void kb_handler(void);

#endif /* _KERNEL_KEYBOARD_H */

