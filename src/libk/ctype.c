
#include <ctype.h>

int tolower(int c) {
    if (isupper(c))
        c -= 'A' - 'a';
    return c;
}

int toupper(int c) {
    if (islower(c))
        c -= 'a' - 'A';
    return c;
}
