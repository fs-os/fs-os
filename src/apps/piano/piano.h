
#ifndef _APPS_PIANO_H
#define _APPS_PIANO_H

#include <stdint.h>

typedef struct {
    uint8_t ch;    /* Key on the user's keyboard */
    uint32_t freq; /* Frequency of the pc speaker for this key */
    bool pressed;  /* The key was pressed in the current iteration */
    bool held;     /* The key is being held since the last iteration */
} piano_key;

/* piano_main: main piano function */
void piano_main(void);

#endif /* _APPS_PIANO_H */

