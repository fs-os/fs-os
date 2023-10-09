
#ifndef STDIO_H_
#define STDIO_H_ 1

#include <stdarg.h>
#include <stdint.h>

/**
 * @def EOF
 * @brief End of file.
 */
#define EOF (-1)

/**
 * @typedef FILE
 * @todo Actual FILE struct: https://www.geeksforgeeks.org/data-type-file-c/
 */
typedef uint32_t FILE;

/**
 * @name File descriptors
 * @{ */
#define stdin  (FILE*)0 /**< @brief Standard input */
#define stdout (FILE*)1 /**< @brief Standard output */
#define stderr (FILE*)2 /**< @brief Standard error */
/** @} */

/**
 * @brief Prints with the specified format using the specified variable argument
 * list.
 *
 * Formats supported:
 *   - "%%"
 *   - "%c"
 *   - "%p"
 *   - "%s", "%25s"
 *   - "%d", "%ld", "%lld"
 *   - "%u", "%lu", "%llu"
 *   - "%f", "%lf"
 *   - "%x", "%lx", "%llx", "%X", "%lX", "%llX"
 *   - "%025d", "%25d", "%25ld", "%25lld"
 *   - "%025u", "%25u", "%25lu", "%25llu"
 *   - "%025f", "%25f", "%25lf", "%.3f", "%25.3f"
 *   - "%025x", "%25x", "%25lx", "%25llx", "%025X", "%25X", "%25lX", "%25llX"
 *
 * Note that using "%025..." instead of "%25..." changes the padding character
 * from ' ' to '0'.
 *
 * @param[in] fmt Format string.
 * @param[in] va Variable argument list.
 * @return Bytes written.
 */
int vprintf(const char* fmt, va_list va);

/**
 * @brief Prints the specified string and a newline char.
 * @param[in] str Zero-terminated string to print.
 * @return 1 if success, EOF otherwise.
 * @todo fputs()
 */
int puts(const char* str);

/**
 * @brief Write to the specified stream with the specified format.
 * @details For now, only `stdin` and `stdout` are supported, and the change is
 * just visual.
 * @param[out] stream Stream for writing.
 * @param[in] fmt Format string.
 * @return Bytes written.
 */
int fprintf(FILE* stream, const char* fmt, ...)
  __attribute__((format(printf, 2, 3)));

/**
 * @brief Write to the specified stream with the specified format using the
 * specified variable argument list.
 * @details For now, only `stdin` and `stdout` are supported, and the change is
 * just visual.
 * @param[out] stream Stream for writing.
 * @param[in] fmt Format string.
 * @param[in] va Variable argument list.
 * @return Bytes written.
 */
int vfprintf(FILE* stream, const char* fmt, va_list va);

/**
 * @brief Prints with the specified format.
 * @param[in] fmt Format string.
 * @return Bytes written.
 */
int printf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Prints the specified character.
 * @param[in] c Character to print.
 * @return Printed character.
 */
int putchar(int c);

/**
 * @brief Get the next char from the user input.
 * @details Wrapper for kb_getchar(). See kb_getchar() function for more
 * information.
 * @return Next character from user input.
 */
int getchar(void);

#endif /* STDIO_H_ */
