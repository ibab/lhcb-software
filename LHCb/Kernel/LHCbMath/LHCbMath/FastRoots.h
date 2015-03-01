/** @file FastRoots.h
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-02-25
 *
 * fast (approximate) root implementations
 *
 * typically, these implementations are accurate up to a couple of least
 * significant bits in the mantissa (where couple is somewhere between 0 and 8)
 */

#ifndef FASTROOTS_H
#define FASTROOTS_H

#include <cmath>
#include <cstdint>
#include <algorithm>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
// all okay, code will work (tested on x86_64/linux)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
// all okay, code will work (tested on powerpc/linux)
#else
#error "Unknown endianness, please define __LITTLE_ENDIAN__ or __BIG_ENDIAN__."
#endif
#if __FLOAT_WORD_ORDER != __BYTE_ORDER__
#error "Floating point and integer byte order do not match!"
#endif

// hint to gcc's optimizer that __pure functions have no side effects and
// depend only on declared input arguments
#ifdef __pure
#undef __pure
#endif
#if defined(__GNUC__)
#define __pure __attribute__((const))
#else
#define __pure
#endif

/** @brief namespace to hold fast root implementations
 *
 * Most of the routines here are based on what is known as the "fast inverse
 * square root trick". The trick exploits that the bit pattern of a floating
 * point number, when interpreted as an integer, is (up to a couple of tweaks)
 * a good approximation of its logarithm. One thus proceeds as follows:
 *
 * - reinterpret float/double as int32_t/int64_t
 * - add/subtract multiple of integer to/from magic constant
 * - reinterpret int32_t/int64_t as float/double
 * - run a couple of iterations of Newton's method to refine the guess
 *
 * The magic number can be calculated as 
 *
 * (1 - p) * M * (B - 0.0450465)
 *
 * where M = 2^23 for float and M = 2^52 for double, and B = 127 for float and
 * B = 1023 for double, and p is the power to which a number should be raised
 * (p in [-1, 1]).  To compute a cube root in float, the magic number is 2/3 *
 * 2^23 * (127 - 0.0450465) = 0x2a517d47; the first guess of the cube root is
 * thus obtained as the float one gets from the bit pattern of 0x2a517d47 +
 * reinterpret_cast<int32_t>(x) / 3.
 *
 * By refining with a couple of iterations of Newton's method, this crude
 * approximation can be improved to the level that the floating point type
 * will allow.
 *
 * There is no implementation of the square root itself because modern
 * x86(_64) processors have instructions to compute square roots in their SSE
 * instruction set which are so fast that they are essentially impossible to
 * beat with the trick above while remaining reasonably accurate.
 */
namespace FastRoots {
// silence gcc's, clang's and icc's warnings about strict aliasing, since we
// play dirty tricks with the binary representation of floats and doubles
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    // forward decls
    inline float cbrt(float x) noexcept __pure;
    inline double cbrt(double x) noexcept __pure;
    inline float invcbrt(float x) noexcept __pure;
    inline double invcbrt(double x) noexcept __pure;
    inline float fifthroot(float x) noexcept __pure;
    inline double fifthroot(double x) noexcept __pure;
    inline float invfifthroot(float x) noexcept __pure;
    inline double invfifthroot(double x) noexcept __pure;

    inline float veryfast_hypot(float x, float y) noexcept __pure;
    inline double veryfast_hypot(double x, double y) noexcept __pure;

    /** @brief calculate cube root
     *
     * @param x	number of which cube root should be computed
     * @returns cube root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline float cbrt(float x) noexcept
    {
	const float xa = std::abs(x);
	int32_t i = reinterpret_cast<const int32_t&>(xa);
	i = 0x2a517d47 + i / 3;
	float r = reinterpret_cast<float&>(i);
	r -= (r - xa / (r * r)) / 3;
	r -= (r - xa / (r * r)) / 3;
	r -= (r - xa / (r * r)) / 3;
	return std::copysign(r, x);
    }

    /** @brief calculate cube root
     *
     * @param x	number of which cube root should be computed
     * @returns cube root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline double cbrt(double x) noexcept
    {
	const double xa = std::abs(x);
	int64_t i = reinterpret_cast<const int64_t&>(xa);
	i = 0x2a9f84fe36d22425 + i / 3;
	double r = reinterpret_cast<double&>(i);
	r -= (r - xa / (r * r)) / 3;
	r -= (r - xa / (r * r)) / 3;
	r -= (r - xa / (r * r)) / 3;
	r -= (r - xa / (r * r)) / 3;
	return std::copysign(r, x);
    }

    /** @brief calculate inverse cube root
     *
     * @param x	number of which inverse cube root should be computed
     * @returns inverse cube root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline float invcbrt(float x) noexcept
    {
	const float xa = std::abs(x);
	int32_t i = reinterpret_cast<const int32_t&>(xa);
	i = 0x54a2fa8e - i / 3;
	float r = reinterpret_cast<float&>(i);
	r += (r - xa * ((r * r) * (r * r))) / 3;
	r += (r - xa * ((r * r) * (r * r))) / 3;
	r += (r - xa * ((r * r) * (r * r))) / 3;
	return std::copysign(r, x);
    }

    /** @brief calculate inverse cube root
     *
     * @param x	number of which inverse cube root should be computed
     * @returns inverse cube root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline double invcbrt(double x) noexcept
    {
	const double xa = std::abs(x);
	int64_t i = reinterpret_cast<const int64_t&>(xa);
	i = 0x553f09fc6da44849 - i / 3;
	double r = reinterpret_cast<double&>(i);
	r += (r - xa * ((r * r) * (r * r))) / 3;
	r += (r - xa * ((r * r) * (r * r))) / 3;
	r += (r - xa * ((r * r) * (r * r))) / 3;
	r += (r - xa * ((r * r) * (r * r))) / 3;
	return std::copysign(r, x);
    }

    /** @brief calculate fifth root
     *
     * @param x	number of which fifth root should be computed
     * @returns fifth root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline float fifthroot(float x) noexcept
    {
	const float xa = std::abs(x);
	int32_t i = reinterpret_cast<const int32_t&>(xa);
	i = 0x32c82fee + i / 5;
	float r = reinterpret_cast<float&>(i);
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	return std::copysign(r, x);
    }

    /** @brief calculate fifth root
     *
     * @param x	number of which fifth root should be computed
     * @returns fifth root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline double fifthroot(double x) noexcept
    {
	const double xa = std::abs(x);
	int64_t i = reinterpret_cast<const int64_t&>(xa);
	i = 0x3325d2caa82f5e92 + i / 5;
	double r = reinterpret_cast<double&>(i);
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	r -= (r - xa / ((r * r) * (r * r))) / 5;
	return std::copysign(r, x);
    }
    
    /** @brief calculate inverse fifth root
     *
     * @param x	number of which inverse fifth root should be computed
     * @returns inverse fifth root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline float invfifthroot(float x) noexcept
    {
	const float xa = std::abs(x);
	int32_t i = reinterpret_cast<const int32_t&>(xa);
	i = 0x4c2c47e6 - i / 5;
	float r = reinterpret_cast<float&>(i);
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	return std::copysign(r, x);
    }

    /** @brief calculate inverse fifth root
     *
     * @param x	number of which inverse fifth root should be computed
     * @returns inverse fifth root of x
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     */
    inline double invfifthroot(double x) noexcept
    {
	const double xa = std::abs(x);
	int64_t i = reinterpret_cast<const int64_t&>(xa);
	i = 0x4cb8bc2ffc470ddb - i / 5;
	double r = reinterpret_cast<double&>(i);
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	r += (r - xa * ((r * r) * (r * r) * (r * r))) / 5;
	return std::copysign(r, x);
    }

#pragma GCC diagnostic pop // end silence strict aliasing warning

    /** @brief very fast approximation of hypotenuse of a triangle
     *
     * @param x	length of one side of triangle
     * @param y	length of other side of triangle
     * @returns approximation of sqrt(x^2 + y^2)
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     *
     * @note this approximation is accurate to about 3.96%.
     */
    inline float hypot(float x, float y) noexcept
    {
	x = std::abs(x), y = std::abs(y);
	return 0.96043387010341996525f * std::max(x, y) +
	    0.39782473475931601382f * std::min(x, y);
    }

    /** @brief very fast approximation of hypotenuse of a triangle
     *
     * @param x	length of one side of triangle
     * @param y	length of other side of triangle
     * @returns approximation of sqrt(x^2 + y^2)
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-02-25
     *
     * @note this approximation is accurate to about 3.96%.
     */
    inline double hypot(double x, double y) noexcept
    {
	x = std::abs(x), y = std::abs(y);
	return 0.96043387010341996525 * std::max(x, y) +
	    0.39782473475931601382 * std::min(x, y);
    }
}

#undef __pure

#endif // FASTROOTS_H

// vim: sw=4:tw=78:ft=cpp
