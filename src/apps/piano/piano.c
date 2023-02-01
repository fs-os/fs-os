
#include <stdio.h>
#include <stddef.h>

#include <kernel/keyboard.h> /* kb_noecho, kb_held */
#include <kernel/pcspkr.h>   /* pcspkr_play, pcspkr_clear */

#include "piano.h"

#define EXIT_CH 'q'

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

static inline void print_piano(void) {
    const char* piano_layout[] = {
        "\xC9\xCD\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCD\xCB\xCD\xCD\xCB\xCD\xCB"
        "\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCD\xCB\xCD\xCD\xBB",

        "\xBA  \xBA"
        "E\xBA \xBAR\xBA  \xBA  \xBAY\xBA \xBAU\xBA \xBAI\xBA  \xBA  \xBA",

        "\xBA  \xC8\xCB\xBC \xC8\xCB\xBC  \xBA  \xC8\xCB\xBC \xC8\xCB\xBC "
        "\xC8\xCB\xBC  \xBA  \xC8\xBB",

        "\xBA S \xBA D \xBA F \xBA G \xBA H \xBA J \xBA K \xBA L \xBA",

        "\xC8\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xCA\xCD"
        "\xCD\xCD\xCA\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xBC",
    };

    for (size_t i = 0; i < LENGTH(piano_layout); i++) {
        putchar('\t');
        puts(piano_layout[i]);
    }
}

static piano_key piano_keys[] = {
    { 's', 261, false }, /* C  | freq: .6 */
    { 'e', 277, false }, /* C# | freq: .2 */
    { 'd', 293, false }, /* D  | freq: .7 */
    { 'r', 311, false }, /* D# | freq: .1 */
    { 'f', 329, false }, /* E  | freq: .6 */
    { 'g', 349, false }, /* F  | freq: .2 */
    { 'y', 370, false }, /* F# | freq: .0 */
    { 'h', 392, false }, /* G  | freq: .0 */
    { 'u', 415, false }, /* G# | freq: .3 */
    { 'j', 440, false }, /* A  | freq: .0 */
    { 'i', 466, false }, /* A# | freq: .2 */
    { 'k', 493, false }, /* B  | freq: .9 */
    { 'l', 523, false }, /* C  | freq: .2 */

};

/* piano_main: main piano function */
void piano_main(void) {
    const bool restore_echo = kb_getecho();

    kb_noecho();
    print_piano();

    /* Main piano loop */
    while (!kb_held(EXIT_CH)) {
        /* Used to check if we are playing a note at all */
        bool playing = false;

        for (size_t i = 0; i < LENGTH(piano_keys); i++) {
            const bool keyboard_held = kb_held(piano_keys[i].ch);

            if (keyboard_held && !piano_keys[i].pressed) {
                /* The key was just pressed */
                piano_keys[i].pressed = true;
                pcspkr_play(piano_keys[i].freq);
            } else if (!keyboard_held && piano_keys[i].pressed) {
                /* The key just released */
                piano_keys[i].pressed = false;
            }

            if (piano_keys[i].pressed)
                playing = true;
        }

        /* If we just pressed a key, give that key priority */

        /* If we are not playing any note, stop the pc speaker */
        if (!playing)
            pcspkr_clear();
    }

    /* If we were echoing the keyboard before the program, restore it */
    if (restore_echo)
        kb_echo();
}

