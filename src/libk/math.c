#include <math.h>

double abs(double x) {
    return x < 0 ? -x : x;
}

double sqrt(double x) {
    double result;

    asm volatile("sqrtsd %0, %1" : "=x" (result) : "x" (x));

    return result;
}

double sin(double x) {
    double result;

    asm volatile("fld %1\n\t"
                 "fsin\n\t"
                 "fstp %0"
                 : "=m" (result)
                 : "m" (x));

    return result;
}

double cos(double x) {
    double result;

    asm volatile("fld %1\n\t"
                 "fcos\n\t"
                 "fstp %0"
                 : "=m" (result)
                 : "m" (x));

    return result;
}

double tan(double x) {
    double result;

    asm volatile("fld %1\n\t"
                 "fsincos\n\t"
                 "fdivp\n\t"
                 "fstp %0"
                 : "=m" (result)
                 : "m" (x));

    return result;
}

double cot(double x) {
    double result;

    asm volatile("fld %1\n\t"
                 "fsincos\n\t"
                 "fdivrp\n\t"
                 "fstp %0"
                 : "=m" (result)
                 : "m" (x));

    return result;
}

double pow(double base, int exponent) {
    double result = 1;

    while (exponent--)
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
