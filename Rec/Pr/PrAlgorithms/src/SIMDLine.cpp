/** @file SIMDLine.cpp
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2016-01-06
 *
 * SIMDised straight line fits - implementations for routines too long to
 * inline.
 */

#include "SIMDLine.h"

float SIMDLine::chi2(const float* dz, const float* dzend,
	const float* w, const float* x) const noexcept
{
    // for properly aligned data:
    // - 8 VFLOPS per 4 elements + 4 VFLOPS for final hadd
    // for arbitrary alignment:
    // - (7 VFLOPS + 1 FLOP) per element
    float retVal = 0.f;
    // check for dz, w and x correct alignment wrt SIMD size:
    // - all aligned wrt to a multiple of sizeof(float)
    // - potential misalignment wrt. to sizeof(FloatVec) is same
    //   for dz, w and x
    if (((reinterpret_cast<const char*>(dz) - reinterpret_cast<const char*>(w)) &
		(sizeof(FloatVec) - 1)) == ((reinterpret_cast<const char*>(dz) -
			reinterpret_cast<const char*>(x)) & (sizeof(FloatVec)
			- 1)) && 0 == ((reinterpret_cast<const char*>(dz) -
			    static_cast<const char*>(nullptr)) &
			(sizeof(float) - 1))) {
	constexpr auto vToS = sizeof(FloatVec) / sizeof(float);
	static_assert(4 == vToS, "alignment constraint violated");
	// get to right alignment for vectorised version
	switch ((dz - static_cast<const float*>(nullptr)) & (vToS - 1)) {
	    case 3: retVal += chi2(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 2: retVal += chi2(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 1: retVal += chi2(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 0: default: break;
	}
	/// calculate end address for vectorised version
	const auto inc = (dzend - dz) / vToS;
	if (inc) {
	    // work vectorised (4 measurements at a time)
	    retVal += chi2_aligned(
		    reinterpret_cast<const FloatVec*>(dz),
		    reinterpret_cast<const FloatVec*>(dz) + inc,
		    reinterpret_cast<const FloatVec*>(w),
		    reinterpret_cast<const FloatVec*>(x));
	    // increment pointers
	    dz += vToS * inc, w += vToS * inc, x += vToS * inc;
	}
    }
    // loop over any remaining measurements
    while (dzend != dz) {
	// unroll loop
	auto inc = dzend - dz;
	switch (inc) {
	    default:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		retVal += chi2(dz[3], w[3], x[3]);
		retVal += chi2(dz[4], w[4], x[4]);
		retVal += chi2(dz[5], w[5], x[5]);
		retVal += chi2(dz[6], w[6], x[6]);
		retVal += chi2(dz[7], w[7], x[7]);
		break;
	    case 7:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		retVal += chi2(dz[3], w[3], x[3]);
		retVal += chi2(dz[4], w[4], x[4]);
		retVal += chi2(dz[5], w[5], x[5]);
		retVal += chi2(dz[6], w[6], x[6]);
		break;
	    case 6:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		retVal += chi2(dz[3], w[3], x[3]);
		retVal += chi2(dz[4], w[4], x[4]);
		retVal += chi2(dz[5], w[5], x[5]);
		break;
	    case 5:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		retVal += chi2(dz[3], w[3], x[3]);
		retVal += chi2(dz[4], w[4], x[4]);
		break;
	    case 4:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		retVal += chi2(dz[3], w[3], x[3]);
		break;
	    case 3:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		retVal += chi2(dz[2], w[2], x[2]);
		break;
	    case 2:
		retVal += chi2(dz[0], w[0], x[0]);
		retVal += chi2(dz[1], w[1], x[1]);
		break;
	    case 1:
		retVal += chi2(dz[0], w[0], x[0]);
	}
	if (inc > 8) inc = 8;
	dz += inc, w += inc, x += inc;
    }
    // done, return result
    return retVal;
}

SIMDLine SIMDLine::combine(const SIMDLine& other) const noexcept
{
    // 62 VFLOPS
    const auto ic1 = invert(m_par[1]), ic2 = invert(other.m_par[1]);
    const auto wp = mulMV(ic1, m_par[0]) + mulMV(ic2, other.m_par[0]);
    const auto iicsum = invert(ic1 + ic2);
    return SIMDLine(mulMV(iicsum, wp), iicsum);
}

void SIMDLineFit::addHits(const float* dz, const float* dzend,
	const float* w, const float* x) noexcept
{
    // check for dz, w and x correct alignment wrt SIMD size:
    // - all aligned wrt to a multiple of sizeof(float)
    // - potential misalignment wrt. to sizeof(FloatVec) is same
    //   for dz, w and x
    if (((reinterpret_cast<const char*>(dz) - reinterpret_cast<const char*>(w)) &
		(sizeof(FloatVec) - 1)) == ((reinterpret_cast<const char*>(dz) -
			reinterpret_cast<const char*>(x)) & (sizeof(FloatVec)
			- 1)) && 0 == ((reinterpret_cast<const char*>(dz) -
			    static_cast<const char*>(nullptr)) &
			(sizeof(float) - 1))) {
	constexpr auto vToS = sizeof(FloatVec) / sizeof(float);
	static_assert(4 == vToS, "alignment constraint violated");
	// get to right alignment for vectorised version
	switch ((dz - static_cast<const float*>(nullptr)) & (vToS - 1)) {
	    case 3: addHit(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 2: addHit(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 1: addHit(*dz++, *w++, *x++);
		    // fallthrough intended
	    case 0: default: break;
	}
	/// calculate end address for vectorised version
	const auto inc = (dzend - dz) / vToS;
	if (inc) {
	    // work vectorised (4 measurements at a time)
	    addHits_aligned(reinterpret_cast<const FloatVec*>(dz),
		    reinterpret_cast<const FloatVec*>(dz) + inc,
		    reinterpret_cast<const FloatVec*>(w),
		    reinterpret_cast<const FloatVec*>(x));
	    // increment pointers
	    dz += inc * vToS, w += inc * vToS, x += inc * vToS;
	}
    }
    // loop over any remaining measurements
    while (dzend != dz) {
	// unroll loop
	auto inc = dzend - dz;
	switch (inc) {
	    default:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]); addHit(dz[3], w[3], x[3]);
		addHit(dz[4], w[4], x[4]); addHit(dz[5], w[5], x[5]);
		addHit(dz[6], w[6], x[6]); addHit(dz[7], w[7], x[7]);
		break;
	    case 7:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]); addHit(dz[3], w[3], x[3]);
		addHit(dz[4], w[4], x[4]); addHit(dz[5], w[5], x[5]);
		addHit(dz[6], w[6], x[6]);
		break;
	    case 6:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]); addHit(dz[3], w[3], x[3]);
		addHit(dz[4], w[4], x[4]); addHit(dz[5], w[5], x[5]);
		break;
	    case 5:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]); addHit(dz[3], w[3], x[3]);
		addHit(dz[4], w[4], x[4]);
		break;
	    case 4:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]); addHit(dz[3], w[3], x[3]);
		break;
	    case 3:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		addHit(dz[2], w[2], x[2]);
		break;
	    case 2:
		addHit(dz[0], w[0], x[0]); addHit(dz[1], w[1], x[1]);
		break;
	    case 1:
		addHit(dz[0], w[0], x[0]);
	}
	if (inc > 8) inc = 8;
	dz += inc, w += inc, x += inc;
    }
}

// for unit tests and benchmarks
#if defined(UNITTEST)
#include <cmath>
#include <cstdio>
#include <limits>
#include <tuple>

/// return fit data
static std::tuple<const float*, const float*, const float*> data()
{
    static const float __attribute__((aligned(16))) dz[16] = {
	-2.f, -1.f, 1.f, 2.f,
	-4.f, -3.f, 3.f, 4.f,
	-6.f, -5.f, 5.f, 6.f,
	-8.f, -7.f, 7.f, 8.f
    };
    static const float __attribute__((aligned(16))) w[16] = {
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f
    };
    static const float __attribute__((aligned(16))) x[16] = {
	-2.f, -1.f, 1.f, 2.f,
	-4.f, -3.f, 3.f, 4.f,
	-6.f, -5.f, 5.f, 6.f,
	-8.f, -7.f, 7.f, 8.f
    };
    return std::make_tuple(&dz[0], &w[0], &x[0]);
}

/// simple unit test for SIMDLineFit: accumulate measurements, and fit
static SIMDLine doUnitTest1() __attribute__((noinline));
static SIMDLine doUnitTest1()
{
    const float *dz, *w, *x;
    std::tie(dz, w, x) = data();

    SIMDLineFit fit;
    fit.addHits(dz, &dz[16], w, x);
    return fit.solve();
}

/// simple unit test for SIMDLine: calculate chi^2
static float doUnitTest2(const SIMDLine& line) __attribute__((noinline));
static float doUnitTest2(const SIMDLine& line)
{
    const float *dz, *w, *x;
    std::tie(dz, w, x) = data();

    return line.chi2(dz, &dz[16], w, x);
}

/// simple unit test for SIMDLine::combine method
SIMDLine doUnitTest3(const SIMDLine& l) __attribute__((noinline));
SIMDLine doUnitTest3(const SIMDLine& l) { return l.combine(l); }

/// unit test driver
int main()
{
    int retVal = 0;
    // check out fitting
    const auto line = doUnitTest1();
    if (!line.ok()) std::printf("FIT FAILED!\n");
    const auto chi2 = doUnitTest2(line);
    std::printf("FITTED LINE: x = %f tx = %f cov =(%f, %f, %f, %f) chi2 %f\n",
	    line.x(), line.tx(), line.cov(0,0), line.cov(0,1), line.cov(1,0), line.cov(1,1), chi2);
    if (std::abs(line.x() - 0.f) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.tx() - 1.f) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(0, 0) - 0.0625f) > 0.0625f * std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(0, 1) - 0.f) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(1, 0) - 0.f) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(1, 1) - 0.0024509804f) > 0.0024509804f * std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(chi2 - 0.f) > std::numeric_limits<float>::epsilon()) ++retVal;

    const auto line2 = doUnitTest3(line);
    if (std::abs(line.x() - line2.x()) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.tx() - line2.tx()) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(0, 0) - 2.f * line2.cov(0, 0)) > 0.0625f * std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(0, 1) - line2.cov(0, 1)) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(1, 0) - line2.cov(1, 0)) > std::numeric_limits<float>::epsilon()) ++retVal;
    if (std::abs(line.cov(1, 1) - 2.f * line2.cov(1, 1)) > 0.0024509804f * std::numeric_limits<float>::epsilon()) ++retVal;
    return retVal;
}
#endif

// vim: ft=cpp:sw=4:tw=78
