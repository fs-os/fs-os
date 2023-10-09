
#ifndef STRINGS_H_
#define STRINGS_H_ 1

#include <stddef.h>
#include <string.h>

/** @todo Add ffs, index, rindex, strncasecmp */

#define bcmp       memcmp
#define bcopy      memcpy
#define strcasecmp strcmp

/**
 * @brief Set N bytes to zero
 * @details memset wrapper
 * @param[inout] s Pointer to the location
 * @param[in] n The number of bytes to clear
 */
static inline void bzero(void* s, size_t n) {
    memset(s, 0, n);
}

#endif /* STRINGS_H_ */
