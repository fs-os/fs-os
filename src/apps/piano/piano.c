
#include <stdio.h>
#include <stddef.h>

#include <kernel/keyboard.h> /* kb_noecho, kb_held */
#include <kernel/pcspkr.h>   /* pcspkr_play, pcspkr_clear */

#include "piano.h"

#define EXIT_CH 'q'

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

static inline void print_piano(void) {
    const char* piano_layout[] = {
        "\xC9\xCD\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCD\xCB\xCD"
        "\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCB\xCD\xCD\xCB\xCD\xCD\xBB",

        "\xBA  \xBA"
        "E\xBA \xBAR\xBA \xBAT\xBA  \xBA  \xBAU\xBA \xBAI\xBA  \xBA  \xBA",

        "\xBA  \xC8\xCB\xBC \xC8\xCB\xBC \xC8\xCB\xBC  \xBA  \xC8\xCB\xBC "
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
    { 's', "F ", 174, false, false }, /* F  */
    { 'e', "F#", 184, false, false }, /* F# */
    { 'd', "G ", 195, false, false }, /* G  */
    { 'r', "G#", 207, false, false }, /* G# */
    { 'f', "A ", 220, false, false }, /* A  */
    { 't', "A#", 233, false, false }, /* A# */
    { 'g', "B ", 246, false, false }, /* B  */
    { 'h', "C ", 261, false, false }, /* C  */
    { 'u', "C#", 277, false, false }, /* C# */
    { 'j', "D ", 293, false, false }, /* D  */
    { 'i', "D#", 311, false, false }, /* D# */
    { 'k', "E ", 329, false, false }, /* E  */
    { 'l', "F ", 349, false, false }, /* F  */
};

/* piano_main: main piano function */
void piano_main(void) {
    const bool restore_echo = kb_getecho();

    kb_noecho();
    printf("\n\tPress \'%c\' to exit...\n", EXIT_CH);
    print_piano();

    /* Store the frequency of the current playing note */
    piano_key* playing_note = NULL;

    /* Main piano loop */
    while (!kb_held(EXIT_CH)) {
        /* Used to check if we are playing a note at all */
        bool playing = false;

        /* Store which keys are being pressed and held */
        for (size_t i = 0; i < LENGTH(piano_keys); i++) {
            const bool keyboard_held = kb_held(piano_keys[i].ch);

            if (keyboard_held && !piano_keys[i].held) {
                /* The key was just pressed */
                piano_keys[i].pressed = true;
                piano_keys[i].held    = true;

                /* If we just pressed a key, give that key priority */
                playing_note = &piano_keys[i];
                playing      = true;
            } else if (piano_keys[i].held) {
                /* The key is being held (or not) but not for the first time */
                piano_keys[i].pressed = false;

                /* The key just released */
                if (!keyboard_held)
                    piano_keys[i].held = false;
            }
        }

        if (playing_note != NULL && playing_note->held)
            playing = true;

        /* If the key that we were holding was released, iterate again to find
         * another held key */
        if (!playing) {
            for (size_t i = 0; i < LENGTH(piano_keys); i++) {
                if (piano_keys[i].held) {
                    /* If we didn't press a key this iteration, but we are still
                     * holding a key, change the frequency and don't stop the
                     * speaker. We overwrite the freq even before checking the whole
                     * array for pressed keys because we would break inmediately
                     * after we find a pressed key. */
                    playing_note = &piano_keys[i];
                    playing      = true;
                }
            }
        }

        /* If we are not playing any note, stop the pc speaker */
        if (!playing) {
            if (pcspkr_get_freq() != 0) {
                pcspkr_clear();
                printf("\r\tCurrent note:");
            }
        } else if (pcspkr_get_freq() != playing_note->freq) {
            pcspkr_play(playing_note->freq);
            printf("\r\tCurrent note: %s (%ld)", playing_note->note_name,
                   playing_note->freq);
        }
    }

    printf("\r\tGoodbye.\n");

    /* If we were echoing the keyboard before the program, restore it */
    if (restore_echo)
        kb_echo();
}

