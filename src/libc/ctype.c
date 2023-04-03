
#include <ctype.h>

/* tolower: if c is uppercase, returns its lowercase version */
int tolower(int c) {
    if (isupper(c))
        c -= 'A' - 'a';
    return c;
}

/* toupper: if c is lowercase, returns its uppercase version */
int toupper(int c) {
    if (islower(c))
        c -= 'a' - 'A';
    return c;
}

