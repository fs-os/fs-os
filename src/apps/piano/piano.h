
#ifndef APPS_PIANO_H_
#define APPS_PIANO_H_ 1

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

#endif /* APPS_PIANO_H_ */
