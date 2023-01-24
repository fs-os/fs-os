
#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

/* layout: struct containing a pointer to the default and shift layouts for a lang */
typedef struct {
    char* def;
    char* shift;
} layout;

/* kb_handler: actual C handler for the keyboard exceptions recieved from "irq_kb".
 * See src/kernel/idt_asm.asm */
void kb_handler(void);

#endif /* _KERNEL_KEYBOARD_H */

