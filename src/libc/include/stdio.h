
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#define EOF (-1)

/* puts: prints "str" and a newline char */
int puts(const char* str);

/* printf: prints with format "fmt" */
int printf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

/* vprintf: prints with format "fmt" using the variable argument list "va". Formats
 * supported:
 *   - "%c"
 *   - "%s", "%25s"
 *   - "%d", "%l", "%ll", "%ld", "%lld", "%25d"
 *   - "%x", "%lx", "%llx", "%X", "%lX", "%llX"
 *   - "%p"
 *   - "%%"
 */
int vprintf(const char* fmt, va_list va);

/* putchar: prints the single character "c" */
int putchar(int c);

/* getchar: wrapper for kb_getchar */
int getchar(void);

#endif /* _STDIO_H */

