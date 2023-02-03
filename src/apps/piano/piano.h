
#ifndef _APPS_PIANO_H
#define _APPS_PIANO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t ch;      /* Key on the user's keyboard */
    char* note_name; /* String containing the note name, used for displaying only */
    uint32_t freq;   /* Frequency of the pc speaker for this key */
    bool pressed;    /* The key was pressed in the current iteration */
    bool held;       /* The key is being held since the last iteration */
} Piano_note;

/* piano_main: main piano function */
int piano_main(int argc, char** argv);

/* piano_random: play a random song using the specified octave */
int piano_random(int argc, char** argv);

#endif /* _APPS_PIANO_H */

