
#ifndef _APPS_PIANO_H
#define _APPS_PIANO_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Information about each piano note.
 */
typedef struct {
    /** @brief Key on the user's keyboard */
    uint8_t ch;

    /** @brief String containing the note name, used for displaying only */
    char* note_name;

    /** @brief Frequency of the pc speaker for this key */
    uint32_t freq;

    /** @brief The key was pressed in the current iteration */
    bool pressed;

    /** @brief The key is being held since the last iteration */
    bool held;
} Piano_note;

/**
 * @brief Main piano function
 * @param argc, argv Shell arguments
 * @return Exit code
 */
int piano_main(int argc, char** argv);

/**
 * @brief Play a random song using the specified octave
 * @param argc, argv Shell arguments
 * @return Exit code
 */
int piano_random(int argc, char** argv);

#endif /* _APPS_PIANO_H */

