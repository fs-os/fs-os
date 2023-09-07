
#ifndef _APPS_PIANO_H
#define _APPS_PIANO_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Main piano function
 * @param argc, argv Shell arguments
 * @return Exit code
 */
int main_piano(int argc, char** argv);

/**
 * @brief Play a random song using the specified octave
 * @param argc, argv Shell arguments
 * @return Exit code
 */
int main_piano_random(int argc, char** argv);

#endif /* _APPS_PIANO_H */
