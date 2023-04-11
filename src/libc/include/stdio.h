
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

/**
 * @def EOF
 * @brief End of file.
 */
#define EOF (-1)

/**
 * @brief Prints the specified string and a newline char.
 * @param[in] str Zero-terminated string to print.
 * @return 1 if success, EOF otherwise.
 */
int puts(const char* str);

/**
 * @brief Prints with the specified format.
 * @param[in] fmt Format string.
 * @return Bytes written.
 */
int printf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Prints with the specified format using the specified variable argument
 * list.
 *
 * Formats supported:
 *   - "%c"
 *   - "%s", "%25s"
 *   - "%d", "%l", "%ll", "%ld", "%lld"
 *   - "%25d", "%25l", "%25ll", "%25ld", "%25lld"
 *   - "%x", "%lx", "%llx", "%25x", "%25lx", "%25llx"
 *   - "%X", "%lX", "%llX", "%25X", "%25lX", "%25llX"
 *   - "%p"
 *   - "%%"
 *
 * @param[in] fmt Format string.
 * @param[in] va Variable argument list.
 * @return Bytes written.
 */
int vprintf(const char* fmt, va_list va);

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

#endif /* _STDIO_H */
