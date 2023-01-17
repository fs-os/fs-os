
#ifndef _KERNEL_PCSPKR_H
#define _KERNEL_PCSPKR_H

#include <stdint.h>

/* Port used to set the pc speaker in or out */
#define PCSPKR_PORT 0x61

typedef struct {
    uint32_t freq;
    uint32_t ms_len;
} Beep;

/* pcspkr_play: play pc speaker with "freq" frequency (1s format). freq can't be 0 */
void pcspkr_play(uint32_t freq);

/* pcspkr_clear: tell the pc speaker to stop listening from PIT channel 2 */
void pcspkr_clear(void);

/* pcspkr_beep_custom: custom beep with freq and duration using the pc speaker */
void pcspkr_beep_custom(Beep info);

/* pcspkr_beep: simple beep using the pc speaker (wrapper for pcspkr_beep_custom) */
static inline void pcspkr_beep(void) {
    pcspkr_beep_custom((Beep){ 1000, 20 });
}

#endif /* _KERNEL_PCSPKR_H */

