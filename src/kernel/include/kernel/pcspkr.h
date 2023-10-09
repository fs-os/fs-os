
#ifndef KERNEL_PCSPKR_H_
#define KERNEL_PCSPKR_H_ 1

#include <stdint.h>

/**
 * @def PCSPKR_PORT
 * @brief Port used to set the pc speaker in or out.
 */
#define PCSPKR_PORT 0x61

/**
 * @struct Beep
 * @brief Structure for a beep, with a frequency and a duration in ms.
 * @todo Rename `ms_len` to len.
 */
typedef struct {
    uint32_t freq;   /**< @brief Frequency for the PIT (hz) */
    uint32_t ms_len; /**< @brief Duration of the beep (ms) */
} Beep;

/**
 * @brief Play pc speaker with the specified requency (hz).
 * @param[in] freq Frequency. Can't be 0.
 */
void pcspkr_play(uint32_t freq);

/**
 * @brief Tell the pc speaker to stop listening from PIT channel 2.
 */
void pcspkr_clear(void);

/**
 * @brief Get the current frequency used by the pc speaker.
 * @details 0 means disabled.
 * @return The current frequency used by the pc speaker.
 */
uint32_t pcspkr_get_freq(void);

/**
 * @brief Play custom Beep with freq and duration using the pc speaker.
 * @param[in] info Beep structure to play.
 */
void pcspkr_beep_custom(Beep info);

/**
 * @brief Simple beep using the pc speaker.
 * @details Wrapper for pcspkr_beep_custom()
 */
static inline void pcspkr_beep(void) {
    pcspkr_beep_custom((Beep){ 1000, 20 });
}

#endif /* KERNEL_PCSPKR_H_ */
