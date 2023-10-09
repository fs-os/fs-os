
#ifndef MEDIA_SOVIET_ANTHEM_H_
#define MEDIA_SOVIET_ANTHEM_H_ 1

#include <kernel/keyboard.h> /* kb_held */
#include <kernel/pcspkr.h>
#include <kernel/framebuffer_console.h>
#include <kernel/color.h>

Beep soviet_anthem[] = {
    { .freq = 392, .ms_len = 375 },  { .freq = 523, .ms_len = 750 },
    { .freq = 392, .ms_len = 463 },  { .freq = 440, .ms_len = 187 },
    { .freq = 494, .ms_len = 750 },  { .freq = 330, .ms_len = 375 },
    { .freq = 330, .ms_len = 375 },  { .freq = 440, .ms_len = 750 },
    { .freq = 392, .ms_len = 463 },  { .freq = 349, .ms_len = 187 },
    { .freq = 392, .ms_len = 750 },  { .freq = 262, .ms_len = 463 },
    { .freq = 262, .ms_len = 187 },  { .freq = 294, .ms_len = 750 },
    { .freq = 294, .ms_len = 463 },  { .freq = 330, .ms_len = 187 },
    { .freq = 349, .ms_len = 750 },  { .freq = 349, .ms_len = 463 },
    { .freq = 392, .ms_len = 187 },  { .freq = 440, .ms_len = 750 },
    { .freq = 494, .ms_len = 375 },  { .freq = 523, .ms_len = 375 },
    { .freq = 587, .ms_len = 1125 },
};

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief Play the soviet_anthem Beep array.
 */
static inline void play_soviet_anthem(void) {
    fbc_setfore(COLOR_GREEN_B);
    printf("Playing soviet anthem... Press \'q\' to stop.\n");

    for (unsigned long i = 0; i < LENGTH(soviet_anthem) && !kb_held('q'); i++) {
        putchar('\r');

        fbc_setfore(COLOR_WHITE_B);
        putchar('[');
        fbc_setfore(COLOR_GREEN);
        printf("%2d", (unsigned int)i);
        fbc_setfore(COLOR_WHITE_B);
        putchar(']');
        fbc_setfore(COLOR_WHITE);

        printf(" Frequency: %ld, Delay: %ld", soviet_anthem[i].freq,
               soviet_anthem[i].ms_len);
        pcspkr_beep_custom(soviet_anthem[i]);
    }

    putchar('\n');
}

#endif /* MEDIA_SOVIET_ANTHEM_H_ */
