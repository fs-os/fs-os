
#ifndef _KERNEL_PCSPKR_H
#define _KERNEL_PCSPKR_H

#include <stdint.h>

/* Port used to set the pc speaker in or out */
#define PCSPKR_PORT 0x61

/* pcspkr_play: play pc speaker with "freq" frequency (1s format) */
void pcspkr_play(uint32_t freq);

/* pcspkr_clear: tell the pc speaker to stop listening from PIT channel 2 */
void pcspkr_clear(void);

/* pcspkr_beep: simple beep using the pc speaker */
void pcspkr_beep(void);

#endif /* _KERNEL_PCSPKR_H */

