#include <math.h>

int abs(int x) {
    return x < 0 ? -x : x;
}

double pow(double base, int exponent) {
    double result = 1;

    while (exponent-- != 0)
        result *= base;

    return result;
}

int ipow(int base, int exponent) {
    int result = 1;

    while (exponent-- != 0)
        result *= base;

    return result;
}

double floor(double x) {
    int int_part = (int)x;

    if (x < 0 && x != int_part)
        int_part--;

    return int_part;
}

double ceil(double x) {
    int int_part = (int)x;

    if (x > 0 && x != int_part)
        int_part++;

    return int_part;
}

double round(double x) {
    if (x >= 0)
        return floor(x + 0.5);
    else
        return ceil(x - 0.5);
}
