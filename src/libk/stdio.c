
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_VGA
#include <kernel/vga.h>
#else /* Framebuffer console */
#include <kernel/framebuffer_console.h>
#endif

#include <kernel/keyboard.h> /* kb_getchar */

/**
 * @def DEFAULT_DOUBLE_DECIMALS
 * @brief Default decimal places to print with "%f"
 */
#define DEFAULT_DOUBLE_DECIMALS 6

/**
 * @def DEFAULT_PAD_CHAR
 * @brief Default character used for padding "%123d" formats. We can also use
 * "%05d" to temporarily use '0' instead.
 */
#define DEFAULT_PAD_CHAR ' '

/**
 * @var cur_pad_char
 * @brief Current character used by print_pad() for printing paddings of "%123d"
 * formats. Can be overwritten with '0' temporarily in "%05d" formats.
 */
static char cur_pad_char = DEFAULT_PAD_CHAR;

/**
 * @brief Print N ammount of spaces.
 * @param[in] n The number of spaces to print.
 * @return Bytes written.
 */
static inline size_t print_pad(size_t n) {
    for (size_t i = 0; i < n; i++)
        putchar(cur_pad_char);

    /* Reset to default on each call */
    cur_pad_char = DEFAULT_PAD_CHAR;

    return n;
}

/**
 * @brief Prints the speicified string using putchar.
 * @param[in] str Zero terminated string to print.
 * @return Bytes written.
 */
static inline size_t fmt_s(const char* str) {
    size_t i;

    for (i = 0; *str != '\0'; i++)
        putchar(*str++);

    return i;
}

/**
 * @brief Print string with padding.
 * @details Similar to fmt_s(), but adds `strlen(str) - pad` spaces before
 * "str". Used for "%123s"
 * @param[in] str String to print.
 * @param[in] pad Padding for the string.
 * @return Bytes written.
 */
static inline size_t fmt_s_pad(const char* str, size_t pad) {
    size_t ret = 0;

    const size_t len = strlen(str);
    if (len < pad)
        ret += print_pad(pad - len);

    ret += fmt_s(str);

    return ret;
}

/**
 * @brief Convert number to string and print.
 * @details Used by printf's "%d".
 * @param[in] num Number to print.
 * @return Bytes written.
 */
static size_t fmt_d(int64_t num) {
    /* 21 are the digits of ULLONG_MAX */
    static char str[21] = { '\0' };
    itoa(str, num);

    return fmt_s(str);
}

/**
 * @brief Print integer with padding.
 * @details Similar to fmt_d(), but adds `digits(num) - pad` zeros before num.
 * Used for "%123d"
 * @param[in] num Number to print.
 * @param[in] pad Padding for the number.
 * @return Bytes written.
 */
static size_t fmt_d_pad(int64_t num, size_t pad) {
    size_t ret = 0;

    uint32_t digits = digits_int(num);
    if (num < 0)
        digits++; /* The minus sign */

    static char str[21] = { '\0' };
    itoa(str, num);

    if (digits < pad)
        ret += print_pad(pad - digits);

    ret += fmt_s(str);

    return ret;
}

/**
 * @brief Print double.
 * @details Used by vprintf()
 * @param[in] num Number to print.
 * @param[in] decimals Number of decimal places to print.
 * @return Bytes written.
 */
static size_t fmt_f(double num, int decimals) {
    size_t ret = 0;

    /* We need to handle sign ourselves, instead of fmt_d() */
    if (num < 0) {
        putchar('-');
        ret++;

        num = -num;
    }

    /* First print the integer part using printi, and remove it from num for
     * only keeping decimals */
    int32_t int_part = (int32_t)num;
    num -= int_part;

    ret += fmt_d(int_part);

    putchar('.');
    ret++;

    for (int i = 0; i < decimals; i++) {
        num *= 10;

        putchar((int)num + '0');
        ret++;

        num -= (int)num;
    }

    return ret;
}

/**
 * @brief Print double with decimal places and padding.
 * @details Used by vprintf()
 * @param[in] num Number to print.
 * @param[in] pad Padding. Same ass fmt_f(). Used for "%123f"
 * @param[in] num Decimals. Used for "%.5f"
 * @return Bytes written.
 */
static size_t fmt_f_pad(double num, size_t pad, int decimals) {
    size_t ret = 0;

    const size_t digits = digits_double(num, decimals);
    if (digits < pad)
        ret += print_pad(pad - digits);

    ret += fmt_f(num, decimals);

    return ret;
}

/**
 * @brief Print integer in hexadecimal format (lowercase).
 * @param[in] num Number to print in hex format.
 * @param[in] uppercase If true, use lowercase letters.
 * @return Bytes written.
 */
static size_t fmt_x(uint64_t num, bool uppercase) {
    static const char hex_chars_lower[] = "0123456789abcdef";
    static const char hex_chars_upper[] = "0123456789ABCDEF";
    const char* hex_chars = uppercase ? hex_chars_upper : hex_chars_lower;

    if (num == 0) {
        putchar('0');
        return 1;
    }

    static char str[17] = { '\0' };

    int i;
    for (i = 0; num != 0; i++) {
        str[i] = hex_chars[num & 0xF];
        num >>= 4; /* bitsof(0xF); */
    }
    str[i] = '\0';

    strrev(str);
    return fmt_s(str);
}

/**
 * @brief Print integer in hexadecimal format with padding.
 * @details Similar to fmt_x(), but adds `digits(num) - pad` zeros before num.
 * Used for "%123x", "%123X"
 * @param[in] num Number to print in hex format.
 * @param[in] pad Padding for the number.
 * @param[in] uppercase If true will use uppercase hex chars.
 * @return Bytes written.
 */
static size_t fmt_x_pad(int64_t num, size_t pad, bool uppercase) {
    size_t ret = 0;

    const size_t digits = digits_hex(num);
    if (digits < pad)
        ret += print_pad(pad - digits);

    ret += fmt_x(num, uppercase);

    return ret;
}

/**
 * @brief Print the address of the specified pointer in hex format.
 * @details Prints "(null)" if NULL.
 * @param[in] ptr Pointer to print.
 * @return Bytes written.
 */
static size_t fmt_p(void* ptr) {
    if (ptr == NULL)
        return fmt_s("(null)");

    size_t ret = 0;

    ret += fmt_s("0x");
    ret += fmt_x((uint32_t)ptr, true);

    return ret;
}

int vprintf(const char* restrict fmt, va_list va) {
    int written = 0;

    while (*fmt != '\0') {
        /* If the current char is not '%', just print */
        if (*fmt != '%') {
            putchar(*fmt++);
            written++;

            if (written >= INT_MAX)
                return -1; /**< @todo Set errno to EOVERFLOW */

            continue;
        }

        /* Rest of the loop is for the "%..." formats. First of all, skip '%' */
        fmt++;

        switch (*fmt) {
            case 'c': /* "%c" */
                putchar((char)va_arg(va, int));
                written++;
                break;
            case 's': /* "%s" */
                written += fmt_s(va_arg(va, const char*));
                break;
            case 'd': /* "%d" */
                written += fmt_d(va_arg(va, int));
                break;
            case 'u': /* %u */
                written += fmt_d(va_arg(va, unsigned int));
                break;
            case 'f': /* %f */
                /* Floats get promoted to doubles when calling printf */
                written += fmt_f(va_arg(va, double), DEFAULT_DOUBLE_DECIMALS);
                break;
            case 'x': /* "%x" */
                written += fmt_x(va_arg(va, unsigned int), false);
                break;
            case 'X': /* "%X" */
                written += fmt_x(va_arg(va, unsigned int), true);
                break;
            case 'p': /* "%p" */
                written += fmt_p(va_arg(va, void*));
                break;
            case '.':  /* "%.123f" */
                fmt++; /* Skip the dot */

                int dot_fmt_num = 0;
                do {
                    dot_fmt_num *= 10;
                    dot_fmt_num += *fmt - '0';
                    fmt++;
                } while (*fmt >= '0' && *fmt <= '9');

                /* Should not happen */
                if (*fmt != 'f')
                    break;

                written += fmt_f(va_arg(va, double), dot_fmt_num);
                break;
            case 'l':  /* "%l..." */
                fmt++; /* Skip the first 'l' */

                /* Check pattern  */
                switch (*fmt) {
                    default: /* "%l?" -> "%ld" */
                        /* If it is an unknown char, subtract one and print int.
                         * We subtract one here because we want to analize the
                         * char on the next iteration, and it will get increased
                         * after the format switch. */
                        fmt--;
                        /* fall through */
                    case 'd': /* "%ld" */
                        written += fmt_d(va_arg(va, long));
                        break;
                    case 'u': /* "%lu" */
                        written += fmt_d(va_arg(va, unsigned long));
                        break;
                    case 'f': /* "%lf" */
                        /* Same as "%f", see comment there */
                        written +=
                          fmt_f(va_arg(va, double), DEFAULT_DOUBLE_DECIMALS);
                        break;
                    case 'x': /* "%lx" */
                        written += fmt_x(va_arg(va, unsigned long), false);
                        break;
                    case 'X': /* "%lX" */
                        written += fmt_x(va_arg(va, unsigned long), true);
                        break;
                    case 'l':  /* "%ll..." */
                        fmt++; /* Skip the second 'l' */

                        switch (*fmt) {
                            default: /* "%ll?" -> "%lld" */
                                /* See previous comment on "%ld" */
                                fmt--;
                                /* fall through */
                            case 'd': /* "%lld" */
                                written += fmt_d(va_arg(va, long long));
                                break;
                            case 'u': /* "%llu" */
                                written +=
                                  fmt_d(va_arg(va, unsigned long long));
                                break;
                            case 'x': /* "%llx" */
                                written +=
                                  fmt_x(va_arg(va, unsigned long long), false);
                                break;
                            case 'X': /* "%llX" */
                                written +=
                                  fmt_x(va_arg(va, unsigned long long), true);
                                break;
                        } /* %lld switch */
                        break;
                } /* %ld switch */
                break;
            case '0': /* "%0..." */
                /* If the padding starts with zero, we use '0' as pad char */
                cur_pad_char = '0';

                /* Skip the zero and read normal format */
                fmt++;

                /* fall through */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': /* "%123..." */
                /* Read all the numbers from the format */
                int fmt_num = 0;
                do {
                    fmt_num *= 10;
                    fmt_num += *fmt - '0';
                    fmt++;
                } while (*fmt >= '0' && *fmt <= '9');

                /* Now fmt points to the format char:
                 *  "%123?"
                 *       ^  */
                switch (*fmt) {
                    case 'd': /* "%123d" */
                        written += fmt_d_pad(va_arg(va, int), fmt_num);
                        break;
                    case 'u': /* "%123u" */
                        written += fmt_d_pad(va_arg(va, unsigned int), fmt_num);
                        break;
                    case 'f': /* "%123f" */
                        written += fmt_f_pad(va_arg(va, double), fmt_num,
                                             DEFAULT_DOUBLE_DECIMALS);
                        break;
                    case 'x': /* "%123x" */
                        written +=
                          fmt_x_pad(va_arg(va, unsigned int), fmt_num, false);
                        break;
                    case 'X': /* "%123X" */
                        written +=
                          fmt_x_pad(va_arg(va, unsigned int), fmt_num, true);
                        break;
                    case 's': /* "%132s" */
                        written += fmt_s_pad(va_arg(va, const char*), fmt_num);
                        break;
                    case '.': /* "%123.5f" */
                        /* Skip the dot */
                        fmt++;

                        int dot_fmt_num = 0;
                        do {
                            dot_fmt_num *= 10;
                            dot_fmt_num += *fmt - '0';
                            fmt++;
                        } while (*fmt >= '0' && *fmt <= '9');

                        /* Should not happen */
                        if (*fmt != 'f')
                            break;

                        written +=
                          fmt_f_pad(va_arg(va, double), fmt_num, dot_fmt_num);
                        break;
                    case 'l':  /* "%123l..." */
                        fmt++; /* Skip the first 'l' of "%123l?" */

                        switch (*fmt) {
                            default: /* "%123l?" -> "%123ld" */
                                /* See previous comment on "%ld" */
                                fmt--;
                                /* fall through */
                            case 'd': /* "%123ld" */
                                written += fmt_d_pad(va_arg(va, long), fmt_num);
                                break;
                            case 'u': /* "%123lu" */
                                written +=
                                  fmt_d_pad(va_arg(va, unsigned long), fmt_num);
                                break;
                            case 'f': /* "%123lf" */
                                /* Same as "%123f". No "%123.5lf" format */
                                written +=
                                  fmt_f_pad(va_arg(va, double), fmt_num,
                                            DEFAULT_DOUBLE_DECIMALS);
                                break;
                            case 'x': /* "%123lx" */
                                written += fmt_x_pad(va_arg(va, unsigned long),
                                                     fmt_num, false);
                                break;
                            case 'X': /* "%123lX" */
                                written += fmt_x_pad(va_arg(va, unsigned long),
                                                     fmt_num, true);
                                break;
                            case 'l': /* "%123ll..." */
                                /* Skip the second 'l' of "%123ll?" */
                                fmt++;

                                switch (*fmt) {
                                    default: /* "%123ll?" -> "%123lld" */
                                        /* See previous comment on "%ld" */
                                        fmt--;
                                        /* fall through */
                                    case 'd': /* "%123lld" */
                                        written += fmt_d_pad(
                                          va_arg(va, long long), fmt_num);
                                        break;
                                    case 'u': /* "%123llu" */
                                        written += fmt_d_pad(
                                          va_arg(va, unsigned long long),
                                          fmt_num);
                                        break;
                                    case 'x': /* "%123llx" */
                                        written += fmt_x_pad(
                                          va_arg(va, unsigned long long),
                                          fmt_num, false);
                                        break;
                                    case 'X': /* "%123llX" */
                                        written += fmt_x_pad(
                                          va_arg(va, unsigned long long),
                                          fmt_num, true);
                                        break;
                                } /* %123ll... switch */
                                break;
                        } /* %123l... switch */
                        break;
                    default: /* default case of "%123..." switch */
                        break;
                } /* %123... switch */
                break;
            case '%': /* "%%" -> "%" */
                putchar(*fmt);
                written++;
                break;
            default:
                /* If unknown fmt, print the % and the unknown char */
                putchar('%');
                putchar(*fmt);
                written += 2;
                break;
        } /* Main format char switch */

        fmt++;

        if (written >= INT_MAX)
            return -1; /**< @todo Set errno to EOVERFLOW */
    }

    return written;
}

int puts(const char* str) {
    printf("%s\n", str);
    return 1; /* EOF means failure */
}

int printf(const char* restrict fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vprintf(fmt, va);

    va_end(va);
    return ret;
}

int fprintf(FILE* restrict stream, const char* restrict fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int ret = vfprintf(stream, fmt, va);

    va_end(va);
    return ret;
}

int vfprintf(FILE* restrict stream, const char* restrict fmt, va_list va) {
    if (stream == stderr) {
        /* Save old colors and set fore to red */
        uint32_t old_fg, old_bg;
        fbc_getcols(&old_fg, &old_bg);
        fbc_setfore(COLOR_RED);

        /* Print */
        const int ret = vprintf(fmt, va);

        /* Reset old colors and return bytes written from vprintf */
        fbc_setfore(old_fg);
        return ret;
    } else {
        /* Just call normal vprintf */
        return vprintf(fmt, va);
    }
}

int putchar(int c) {
    const char tmp = (char)c;

#ifdef USE_VGA
    vga_putchar(tmp);
#else /* Framebuffer console */
    fbc_putchar(tmp);
#endif

    return tmp;
}

int getchar(void) {
    return kb_getchar();
}
