
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Default piano notes in octave 3 */
static const Piano_note piano_notes_original[] = {
    /* char, note, freq, pressed, held */
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
int piano_main(int argc, char** argv) {
    /* Make a copy of piano_notes_original so we can edit the octaves there */
    Piano_note piano_notes[LENGTH(piano_notes_original)];
    memcpy(piano_notes, piano_notes_original, sizeof(piano_notes_original));

    /* Arguments */
    if (argc > 1) {
        int octave_arg;
        if ((octave_arg = atoi(argv[1])) == 0) {
            printf("Invalid arguments.\n"
                   "Usage:\n"
                   "\t%s           - Run with default octave (3)\n"
                   "\t%s [octave]  - Run with specific octave (1-5)\n",
                   argv[0], argv[0]);
            return 1;
        }

        switch (octave_arg) {
            case 1:
                for (size_t i = 0; i < LENGTH(piano_notes); i++)
                    piano_notes[i].freq /= 4;
                break;
            case 2:
                for (size_t i = 0; i < LENGTH(piano_notes); i++)
                    piano_notes[i].freq /= 2;
                break;
            case 3:
                break;
            case 4:
                for (size_t i = 0; i < LENGTH(piano_notes); i++)
                    piano_notes[i].freq *= 2;
                break;
            case 5:
                for (size_t i = 0; i < LENGTH(piano_notes); i++)
                    piano_notes[i].freq *= 4;
                break;
            default:
                printf("Invalid octave range.\n"
                       "Usage:\n"
                       "\t%s           - Run with default octave (3)\n"
                       "\t%s [octave]  - Run with specific octave (1-5)\n",
                       argv[0], argv[0]);
                return 1;
        }
    }

    const bool restore_echo = kb_getecho();

    kb_noecho();
    printf("\n\tPress \'%c\' to exit...\n", EXIT_CH);
    print_piano();

    /* Store the frequency of the current playing note */
    Piano_note* playing_note = NULL;

    /* Main piano loop */
    while (!kb_held(EXIT_CH)) {
        /* Used to check if we are playing a note at all */
        bool playing = false;

        /* Store which keys are being pressed (pressed means held for the first time)
         * and held */
        for (size_t i = 0; i < LENGTH(piano_notes); i++) {
            const bool keyboard_held = kb_held(piano_notes[i].ch);

            if (keyboard_held && !piano_notes[i].held) {
                /* The key was just pressed */
                piano_notes[i].pressed = true;
                piano_notes[i].held    = true;

                /* If we just pressed a key, give that note priority */
                playing_note = &piano_notes[i];
                playing      = true;
            } else {
                /* The key is being held (or not) but not for the first time */
                piano_notes[i].pressed = false;

                /* The key just released */
                if (!keyboard_held)
                    piano_notes[i].held = false;
            }
        }

        /* If we didn't find a new pressed key, but the one we were playing is still
         * being held, we keep using that one */
        if (playing_note != NULL && playing_note->held)
            playing = true;

        /* If the key that we were holding was released, iterate again to find
         * another held key */
        if (!playing) {
            for (size_t i = 0; i < LENGTH(piano_notes); i++) {
                if (piano_notes[i].held) {
                    /* If we didn't press a key this iteration, but we are still
                     * holding a key, change the frequency and don't stop the
                     * speaker. We overwrite the freq even before checking the whole
                     * array for pressed keys because we would break inmediately
                     * after we find a pressed key. */
                    playing_note = &piano_notes[i];
                    playing      = true;
                }
            }
        }

#ifdef DEBUG
        for (size_t i = 0; i < LENGTH(piano_notes); i++) {
            if (piano_notes[i].pressed)
                printf("# ");
            else if (piano_notes[i].held)
                printf(". ");
            else
                printf("  ");
        }
        putchar('\n');
#endif

        /* If we are not playing any note, stop the pc speaker */
        if (!playing) {
            if (pcspkr_get_freq() != 0) {
                pcspkr_clear();
#ifndef DEBUG
                printf("\r\tCurrent note:");
#endif
            }
        } else if (pcspkr_get_freq() != playing_note->freq) {
            pcspkr_play(playing_note->freq);
#ifndef DEBUG
            printf("\r\tCurrent note: %s (%ld)", playing_note->note_name,
                   playing_note->freq);
#endif
        }
    }

    printf("\r\tGoodbye.\n");

    /* If we were echoing the keyboard before the program, restore it */
    if (restore_echo)
        kb_echo();

    return 0;
}

