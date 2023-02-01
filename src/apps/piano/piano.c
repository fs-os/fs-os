
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

    putchar('\n');

    for (size_t i = 0; i < LENGTH(piano_layout); i++) {
        putchar('\t');
        puts(piano_layout[i]);
    }

    putchar('\n');
}

static piano_key piano_keys[] = {
    /* char, freq, pressed, held */
    { 's', "C ", 261, false, false }, /* C  | freq: .6 */
    { 'e', "C#", 277, false, false }, /* C# | freq: .2 */
    { 'd', "D ", 293, false, false }, /* D  | freq: .7 */
    { 'r', "D#", 311, false, false }, /* D# | freq: .1 */
    { 'f', "E ", 329, false, false }, /* E  | freq: .6 */
    { 'g', "F ", 349, false, false }, /* F  | freq: .2 */
    { 'y', "F#", 370, false, false }, /* F# | freq: .0 */
    { 'h', "G ", 392, false, false }, /* G  | freq: .0 */
    { 'u', "G#", 415, false, false }, /* G# | freq: .3 */
    { 'j', "A ", 440, false, false }, /* A  | freq: .0 */
    { 'i', "A#", 466, false, false }, /* A# | freq: .2 */
    { 'k', "B ", 493, false, false }, /* B  | freq: .9 */
    { 'l', "C ", 523, false, false }, /* C  | freq: .2 */

};

/* piano_main: main piano function */
void piano_main(void) {
    const bool restore_echo = kb_getecho();

    kb_noecho();
    printf("\n\tPress \'q\' to exit...\n");
    print_piano();

    /* Store the frequency of the current playing note */
    piano_key playing_note = { 0, NULL, 0, false, false };

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
                playing_note = piano_keys[i];
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
                playing_note = piano_keys[i];
                playing      = true;
            }
        }

        /* If we are not playing any note, stop the pc speaker */
        if (!playing) {
            if (pcspkr_get_freq() != 0) {
                pcspkr_clear();
                printf("\r\tCurrent note:");
            }
        } else if (pcspkr_get_freq() != playing_note.freq) {
            pcspkr_play(playing_note.freq);
            printf("\r\tCurrent note: %s (%ld)", playing_note.note_name,
                   playing_note.freq);
        }
    }

    printf("\r\tGoodbye.\n");

    /* If we were echoing the keyboard before the program, restore it */
    if (restore_echo)
        kb_echo();
}

