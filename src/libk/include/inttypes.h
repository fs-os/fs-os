
#ifndef INTTYPES_H_
#define INTTYPES_H_ 1

#include <stddef.h>
#include <stdint.h>

#define __PRI32_PREFIX  "l"
#define __PRI64_PREFIX  "ll"
#define __PRIPTR_PREFIX "l"

#define PRId8  "d"
#define PRId16 "d"
#define PRId32 __PRI32_PREFIX "d"
#define PRId64 __PRI64_PREFIX "d"

/* TODO */
#define PRIi8  "i"
#define PRIi16 "i"
#define PRIi32 __PRI32_PREFIX "i"
#define PRIi64 __PRI64_PREFIX "i"

#define PRIu8  "u"
#define PRIu16 "u"
#define PRIu32 __PRI32_PREFIX "u"
#define PRIu64 __PRI64_PREFIX "u"

/* TODO */
#define PRIo8  "o"
#define PRIo16 "o"
#define PRIo32 __PRI32_PREFIX "o"
#define PRIo64 __PRI64_PREFIX "o"

#define PRIx8  "x"
#define PRIx16 "x"
#define PRIx32 __PRI32_PREFIX "x"
#define PRIx64 __PRI64_PREFIX "x"

#define PRIX8  "X"
#define PRIX16 "X"
#define PRIX32 __PRI32_PREFIX "X"
#define PRIX64 __PRI64_PREFIX "X"

/* Macros for printing `intmax_t` and `uintmax_t`.  */
#define PRIdMAX __PRI64_PREFIX "d"
#define PRIiMAX __PRI64_PREFIX "i"
#define PRIoMAX __PRI64_PREFIX "o"
#define PRIuMAX __PRI64_PREFIX "u"
#define PRIxMAX __PRI64_PREFIX "x"
#define PRIXMAX __PRI64_PREFIX "X"

/* Macros for printing `intptr_t` and `uintptr_t`.  */
#define PRIdPTR __PRIPTR_PREFIX "d"
#define PRIiPTR __PRIPTR_PREFIX "i"
#define PRIoPTR __PRIPTR_PREFIX "o"
#define PRIuPTR __PRIPTR_PREFIX "u"
#define PRIxPTR __PRIPTR_PREFIX "x"
#define PRIXPTR __PRIPTR_PREFIX "X"

#endif /* INTTYPES_H_ */
