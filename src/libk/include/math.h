
#ifndef MATH_H_
#define MATH_H_ 1

#include <stdint.h>

/**
 * @def M_PI
 * @brief The mathematical constant pi.
 */
#define M_PI 3.14159265358979323846

/**
 * @def M_E
 * @brief The mathematical constant e.
 */
#define M_E 2.71828182845904523536

/**
 * @def M_SQRT2
 * @brief The square root of 2.
 */
#define M_SQRT2 1.41421356237309504880

/**
 * @brief Calculates the absolute value of a number.
 * @param[in] x The input number.
 * @return The absolute value of x.
 */
int abs(int x);

/**
 * @brief Calculates the absolute value of a double.
 * @details Defined in math.asm
 * @param[in] x The input number.
 * @return The absolute value of x.
 */
double fabs(double x);

/**
 * @brief Calculates the absolute value of a float.
 * @param[in] x The input number.
 * @return The absolute value of x.
 */
static inline float fabsf(float x) {
    return (x >= 0.f) ? x : -x;
}

/**
 * @brief Calculates the modulus (remainder) of 2 doubles.
 * @param[in] x The dividend.
 * @param[in] x The divisor.
 * @return The remainder of x/y.
 */
static inline double fmod(double x, double y) {
    return x - y * (uint64_t)(x / y);
}

/**
 * @brief Calculates the modulus (remainder) of 2 floats.
 * @param[in] x The dividend.
 * @param[in] x The divisor.
 * @return The remainder of x/y.
 */
static inline float fmodf(float x, float y) {
    return x - y * (uint32_t)(x / y);
}

/**
 * @brief Calculates the square root of a number.
 * @details Defined in math.asm
 * @param[in] x The input number.
 * @return The square root of x.
 */
double sqrt(double x) __attribute__((pure));

/**
 * @brief Calculates the sine of an angle.
 * @details Defined in math.asm
 * @param[in] x The input angle in radians.
 * @return The sine of x.
 */
double sin(double x) __attribute__((pure));

/**
 * @brief Calculates the cosine of an angle.
 * @details Defined in math.asm
 * @param[in] x The input angle in radians.
 * @return The cosine of x.
 */
double cos(double x) __attribute__((pure));

/**
 * @brief Calculates the tangent of an angle.
 * @details Defined in math.asm
 * @param[in] x The input angle in radians.
 * @return The tangent of x.
 */
double tan(double x) __attribute__((pure));

/**
 * @brief Calculates the cotangent of an angle.
 * @details Defined in math.asm
 * @param[in] x The input angle in radians.
 * @return The cotangent of x.
 */
double cot(double x) __attribute__((pure));

/**
 * @brief Calculates the power of a number.
 * @param[in] base The base number.
 * @param[in] exponent The exponent.
 * @return The result of base raised to the power of exponent.
 */
double pow(double base, int exponent) __attribute__((pure));

/**
 * @brief Calculates the power of an integer.
 * @param[in] base The base number.
 * @param[in] exponent The exponent.
 * @return The result of base raised to the power of exponent.
 */
int ipow(int base, int exponent) __attribute__((pure));

/**
 * @brief Rounds a number down to the nearest integer.
 * @param[in] x The input number.
 * @return The nearest integer less than or equal to x.
 */
double floor(double x);

/**
 * @brief Rounds a number up to the nearest integer.
 * @param[in] x The input number.
 * @return The smallest integer greater than or equal to x.
 */
double ceil(double x);

/**
 * @brief Rounds a number to the nearest integer.
 * @param[in] x The input number.
 * @return The nearest integer to x.
 */
double round(double x);

#endif /* MATH_H_ */
