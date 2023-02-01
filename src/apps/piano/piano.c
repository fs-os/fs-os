
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
    /* char, freq, pressed, held */
    { 's', 261, false, false }, /* C  | freq: .6 */
    { 'e', 277, false, false }, /* C# | freq: .2 */
    { 'd', 293, false, false }, /* D  | freq: .7 */
    { 'r', 311, false, false }, /* D# | freq: .1 */
    { 'f', 329, false, false }, /* E  | freq: .6 */
    { 'g', 349, false, false }, /* F  | freq: .2 */
    { 'y', 370, false, false }, /* F# | freq: .0 */
    { 'h', 392, false, false }, /* G  | freq: .0 */
    { 'u', 415, false, false }, /* G# | freq: .3 */
    { 'j', 440, false, false }, /* A  | freq: .0 */
    { 'i', 466, false, false }, /* A# | freq: .2 */
    { 'k', 493, false, false }, /* B  | freq: .9 */
    { 'l', 523, false, false }, /* C  | freq: .2 */

};

/* piano_main: main piano function */
void piano_main(void) {
    const bool restore_echo = kb_getecho();

    kb_noecho();
    print_piano();

    /* Store the frequency of the current playing note */
    uint32_t playing_freq = 0;

    /* Main piano loop */
    while (!kb_held(EXIT_CH)) {
        /* Store which keys are being pressed and held */
        for (size_t i = 0; i < LENGTH(piano_keys); i++) {
            const bool keyboard_held = kb_held(piano_keys[i].ch);

            if (keyboard_held && !piano_keys[i].held) {
                /* The key was just pressed */
                piano_keys[i].pressed = true;
                piano_keys[i].held    = true;

#ifdef DEBUG
                printf("Pressed key \'%c\'\n", piano_keys[i].ch);
#endif
            } else if (piano_keys[i].held) {
                /* The key is being held (or not) but not for the first time */
                piano_keys[i].pressed = false;

                /* The key just released */
                if (!keyboard_held) {
                    piano_keys[i].held = false;

#ifdef DEBUG
                    printf("Released key \'%c\'\n", piano_keys[i].ch);
#endif
                }
            }
        }

        /* Used to check if we are playing a note at all */
        bool playing = false;

        /* Iterate again to see what keys are being pressed or held */
        for (size_t i = 0; i < LENGTH(piano_keys); i++) {
            if (piano_keys[i].pressed) {
                /* If we just pressed a key, give that key priority */
                playing_freq = piano_keys[i].freq;
                playing      = true;

#ifdef DEBUG
                printf("Found pressed key: \'%c\' (%ld)\n", piano_keys[i].ch,
                       piano_keys[i].freq);
#endif
                break;
            } else if (piano_keys[i].held) {
                /* If we didn't press a key this iteration, but we are still holding
                 * a key, change the frequency and don't stop the speaker. We
                 * overwrite the freq even before checking the whole array for
                 * pressed keys because we would break inmediately after we find a
                 * pressed key. */
                playing_freq = piano_keys[i].freq;
                playing      = true;
            }
        }

        /* If we are not playing any note, stop the pc speaker */
        if (playing)
            pcspkr_play(playing_freq);
        else
            pcspkr_clear();
    }

    /* If we were echoing the keyboard before the program, restore it */
    if (restore_echo)
        kb_echo();
}

