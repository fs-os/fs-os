
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#define EOF (-1)

/* puts: prints "str" and a newline char */
int puts(const char* str);

/* printf: prints with format "fmt" */
int printf(const char* fmt, ...);

/* vprintf: prints with format "fmt" using the variable argument list "va" */
int vprintf(const char* fmt, va_list va);

/* putchar: prints the single character "c" */
int putchar(int c);

#endif /* _STDIO_H */

