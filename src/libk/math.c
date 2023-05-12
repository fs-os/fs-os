#include <math.h>

double pow(double base, int exponent) {
    double result = 1;

    while (exponent--)
        result *= base;

    return result;
}

double floor(double x) {
    return (int)x;
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
