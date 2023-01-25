
#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

/* Indexes of the layout.special array. For example:
 *   - en_layout.special will contain the key codes for special chars like shift
 *   - KB_SPECIAL_IDX_LSHIFT is 2 because the second item of the .special array will
 *     be the key code of the left shift key
 *   - en_layout.special[2] is 0x2A because 0x2A is the key code for left shift */
enum kb_special_indexes {
    KB_SPECIAL_IDX_ESC = 0,
    KB_SPECIAL_IDX_LCTRL,
    KB_SPECIAL_IDX_LSHIFT,
    KB_SPECIAL_IDX_RSHIFT,
    KB_SPECIAL_IDX_LALT,
    KB_SPECIAL_IDX_CAPSLOCK,
    KB_SPECIAL_IDX_F1,
    KB_SPECIAL_IDX_F2,
    KB_SPECIAL_IDX_F3,
    KB_SPECIAL_IDX_F4,
    KB_SPECIAL_IDX_F5,
    KB_SPECIAL_IDX_F6,
    KB_SPECIAL_IDX_F7,
    KB_SPECIAL_IDX_F8,
    KB_SPECIAL_IDX_F9,
    KB_SPECIAL_IDX_F10,
    KB_SPECIAL_IDX_NUMLOCK,
    KB_SPECIAL_IDX_SCROLL_LOCK,
    KB_SPECIAL_IDX_HOME,
    KB_SPECIAL_IDX_ARROW_UP,
    KB_SPECIAL_IDX_PAGE_UP,
    KB_SPECIAL_IDX_ARROW_LEFT,
    KB_SPECIAL_IDX_ARROW_RIGHT,
    KB_SPECIAL_IDX_END,
    KB_SPECIAL_IDX_ARROW_DOWN,
    KB_SPECIAL_IDX_PAGE_DOWN,
    KB_SPECIAL_IDX_INSERT,
    KB_SPECIAL_IDX_DELETE,
    KB_SPECIAL_IDX_F11,
    KB_SPECIAL_IDX_F12,
};

/* layout: struct containing a pointer to the default and shift layouts for a lang */
typedef struct {
    char* def;         /* Chars to display for each key code */
    char* shift;       /* Same as .def but when holding shift */
    uint16_t* special; /* Array contaning the indexes of special chars for the
                        * current layout */
} layout;

/* kb_handler: actual C handler for the keyboard exceptions recieved from "irq_kb".
 * See src/kernel/idt_asm.asm */
void kb_handler(void);

/* kb_held: check if "c" is being held. Returns 1 if the first bit of key_flags[c] is
 * set. */
uint8_t kb_held(unsigned char c);

#endif /* _KERNEL_KEYBOARD_H */
