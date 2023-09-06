/**
 * @file math.h
 * @brief Mathematical functions and constants.
 */

#ifndef _MATH_H
#define _MATH_H

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
 * @param[in] x The input number.
 * @return The absolute value of x.
 */
double fabs(double x);

/**
 * @brief Calculates the square root of a number.
 * @param[in] x The input number.
 * @return The square root of x.
 */
double sqrt(double x);

/**
 * @brief Calculates the sine of an angle.
 * @param[in] x The input angle in radians.
 * @return The sine of x.
 *
 * @todo Returns invalid values.
 */
double sin(double x);

/**
 * @brief Calculates the cosine of an angle.
 * @param[in] x The input angle in radians.
 * @return The cosine of x.
 *
 * @todo Returns invalid values.
 */
double cos(double x);

/**
 * @brief Calculates the tangent of an angle.
 * @param[in] x The input angle in radians.
 * @return The tangent of x.
 *
 * @todo Returns invalid values.
 */
double tan(double x);

/**
 * @brief Calculates the cotangent of an angle.
 * @param[in] x The input angle in radians.
 * @return The cotangent of x.
 *
 * @todo Returns invalid values.
 */
double cot(double x);

/**
 * @brief Calculates the power of a number.
 * @param[in] base The base number.
 * @param[in] exponent The exponent.
 * @return The result of base raised to the power of exponent.
 */
double pow(double base, int exponent);

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

#endif /* _MATH_H */
