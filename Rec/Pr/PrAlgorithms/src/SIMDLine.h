/** @file SIMDLine.h
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2016-01-06
 *
 * SIMDised straight line fits.
 */

#ifndef SIMDLINE_H
#define SIMDLINE_H

// give some semblance of portability - gcc and clang work flawlessly, and it
// even on compiles on FreeBSD/UltraSparc (albeit unvectorised, since my
// UltraSparc IIIi CPU doesn't support vectorisation of float quantities)
#ifdef __INTEL_COMPILER
#error "Intel compiler not supported - too difficult to program SIMDised."
#endif // __INTEL_COMPILER
#if defined(__GNUC__)
#if defined(__clang__)
// special hacks required to support clang
#if __has_builtin(__builtin_shufflevector)
/// shuffle SIMD vectors using clang's __builtin_shufflevector
#define simd_shuffle(v, i0, i1, i2, i3) \
    __builtin_shufflevector(v, v, i0, i1, i2, i3)
/// shuffle SIMD vectors using clang's __builtin_shufflevector
#define simd_shuffle2(v, w, i0, i1, i2, i3) \
    __builtin_shufflevector(v, w, i0, i1, i2, i3)
#else // __has_builtin(__builtin_shufflevector)
/// shuffle SIMD vectors, rely on compiler to generate reasonable code
#define simd_shuffle(v, i0, i1, i2, i3) (FloatVec{\
    (v)[i0 & 3], (v)[i1 & 3], (v)[i2 & 3], (v)[i3 & 3] })
/// shuffle SIMD vectors, rely on compiler to generate reasonable code
#define simd_shuffle2(v, w, i0, i1, i2, i3) (FloatVec{ \
	(i0 < 4) ? (v)[i0 & 3] : (w)[i0 & 3], \
	(i1 < 4) ? (v)[i1 & 3] : (w)[i1 & 3], \
	(i2 < 4) ? (v)[i2 & 3] : (w)[i2 & 3], \
	(i3 < 4) ? (v)[i3 & 3] : (w)[i3 & 3] })
#endif // __has_builtin(__builtin_shufflevector)
#else // __clang__
/// shuffle SIMD vectors, use gcc's __builtin_shuffle
#define simd_shuffle(v, i0, i1, i2, i3) \
    __builtin_shuffle(v, IntVec{ i0, i1, i2, i3 })
/// shuffle SIMD vectors, use gcc's __builtin_shuffle
#define simd_shuffle2(v, w, i0, i1, i2, i3) \
    __builtin_shuffle(v, w, IntVec{ i0, i1, i2, i3 })
#endif // __clang__
#endif // __GNUC__

#include <limits>

/** @brief SIMDLineBase class with useful routines for SIMDLine and SIMDLineFit
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2016-01-06
 */
class SIMDLineBase {
    public:
	/// vectorisable type to hold four floats
	typedef float FloatVec __attribute__((vector_size(16), aligned(16)));
	/// vectorisable type to hold four ints
	typedef int IntVec __attribute__((vector_size(16), aligned(16)));

	/// do a horizontal add, return sum in each element of vector
	static inline FloatVec hadd(FloatVec v) noexcept
	{
	    // 4 VFLOPS
	    v += simd_shuffle(v, 2, 3, 0, 1);
	    return v + simd_shuffle(v, 1, 0, 3, 2);
	}

	/// a SIMD implementation of "c ? a : b" - no short circuit semantics!
	static inline FloatVec sel(IntVec c, FloatVec a, FloatVec b)
	{
	    /// 3 VFLOPS, or less
#if defined(__clang__) // clang still needs some gymnastics for this
	    return (FloatVec)((c & ((IntVec) a)) | (~c & ((IntVec) b)));
#else // normal GNU C(++) does support the ternary operator with vector types
	    return c ? a : b;
#endif // defined(__clang__)
	}

	/// compute inverse of m, or return {-max, 0, 0, -max} on failure
	static inline FloatVec invert(FloatVec m) noexcept
	{
	    // 16 VFLOPS (and no branches :)
	    constexpr FloatVec errnumer = {
		-std::numeric_limits<float>::max(), 0.f,
		0.f, -std::numeric_limits<float>::max()
	    }, errdenom = { 1.f, 1.f, 1.f, 1.f }; 
	    const auto numer = simd_shuffle(m, 3, 2, 1, 0) *
		FloatVec{ 1.f, -1.f, -1.f, 1.f };
	    const auto det = FloatVec{ .5f, .5f, .5f, .5f } * hadd(m * numer);
	    const IntVec c = det > FloatVec{ 0.f, 0.f, 0.f, 0.f };
	    return sel(c, numer, errnumer) / sel(c, det, errdenom);
	}

	/// matrix-vector multiply
	static inline FloatVec mulMV(FloatVec m, FloatVec v) noexcept
	{
	    // 4 VFLOPS
	    const auto tmp = m * simd_shuffle(v, 0, 2, 1, 3);
	    return tmp + simd_shuffle(tmp, 1, 0, 3, 2);
	}
};

/** @brief SIMDLine represents a straight line
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2016-01-06
 *
 * A simple straight line representation. Emphasis is put on SIMDised
 * implemenation of the most important routines. Positions along the line can
 * be calculated four at a time, and it features a fully SIMDised version of a
 * routine to calculate the chi^2 of measurement wrt. the line.
 *
 * Use of the routines should be fairly self-explanatory.
 *
 * @note: There is some duplication of information going on in the data
 * members to make the data layout compatible with SIMD operations. Users will
 * not usually see this, but it's a fair point to mention.
 *
 * Example:
 * @code
 * const float dz[12] = { ... }; // 12 measurements: dz_i = z_i - z_ref
 * const float w[12] = { ... };  // 12 measurements: w_i = 1 / sigma_x_i
 * const float x[12] = { ... };  // 12 measurements: x_i
 * SIMDLineFit fit;
 * fit.addHits(dz, &dz[12], w, x);
 * SIMDLine line = fit.solve();
 * if (!line.ok()) {
 *     std::cout << "Line fit failed!" << std::endl;
 * } else {
 *     std::cout << "Line fit: x=" << line.x() << " tx=" << line.tx() <<
 *         " chi^2=" << line.chi2(dz, &dz[12], w, x) << std::endl;
 * }
 * @endcode
 */
class SIMDLine : public SIMDLineBase {
    private:
	/// parameters and covariance ({x0, x0, tx, tx}, {c00, c01, c10, c11})
	FloatVec m_par[2];

    public:
	/// constructor
	inline SIMDLine(float x, float tx,
		float c00 = 0.f, float c01 = 0.f, float c11 = 0.f) noexcept :
	    m_par{ { x, x, tx, tx }, { c00, c01, c01, c11 } }
	{ }

	/// constructor (par = { x, x, tx, tx }, cov = { c00, c01, c01, c11 })
	inline SIMDLine(FloatVec par, FloatVec cov) noexcept : m_par{ par, cov }
	{ }

	/// return if covariance matrix is valid
	inline bool ok() const noexcept { return 0.f <= m_par[1][0]; }

	/// return if covariance matrix is valid
	operator bool() const noexcept { return ok(); }

	/// return x of line
	inline float x() const noexcept { return m_par[0][0]; }
	
	/// return tx of line
	inline float tx() const noexcept { return m_par[0][2]; }

	/// return covariance matrix elements of line parameters
	inline float cov(unsigned i, unsigned j) const noexcept
	{ return m_par[1][2 * i + j]; }

	/// return x for given value of dz
	inline float x(float dz) const noexcept
	{
	    // 3 FLOPS
	    return m_par[0][0] + dz * m_par[0][2];
	}

	/// return x for given values of dz
	inline FloatVec x(FloatVec dz) const noexcept
	{
	    // 4 VFLOPS
	    return simd_shuffle(m_par[0], 0, 1, 0, 1) +
		dz * simd_shuffle(m_par[0], 2, 3, 2, 3);
	}

	/// chi^2 between two Lines (in terms of parameter uncertainties)
	inline float chi2(const SIMDLine& other) const noexcept
	{
	    // 25 VFLOPS
	    const auto dp = m_par[0] - other.m_par[0];
	    const auto C = m_par[1] + other.m_par[1];
	    return hadd(dp * invert(C) * simd_shuffle(dp, 0, 2, 1, 3))[0];
	}

	// calculate chi^2 of a measurement wrt. line parameters
	inline float chi2(float dz, float w, float x) const noexcept
	{
	    // 6 FLOPS
	    const auto tmp = w * (x - this->x(dz));
	    return tmp * tmp;
	}

	// calculate vectorised chi^2 of 4 measurements wrt. line parameters
	inline FloatVec vchi2(
		FloatVec dz, FloatVec w, FloatVec x) const noexcept
	{
	    // 7 VFLOPS
	    const auto tmp = w * (x - this->x(dz));
	    return tmp * tmp;
	}

	/// calculate chi^2 of 4 measurements wrt. line parameters
	inline float chi2(FloatVec dz, FloatVec w, FloatVec x) const noexcept
	{
	    // 11 VFLOPS
	    return hadd(vchi2(dz, w, x))[0];
	}

	/// calculate chi^2 of measurements (aligned arrays)
	float chi2_aligned(const FloatVec* dz, const FloatVec* dzend,
		const FloatVec* w, const FloatVec* x) const noexcept
	{
	    // 8 VFLOPS per element + 4 VFLOPS for final hadd
	    FloatVec retVal = { 0.f, 0.f, 0.f, 0.f };
	    while (dzend != dz) retVal += vchi2(*dz++, *w++, *x++);
	    return hadd(retVal)[0];
	}

	/// calculate chi^2 of measurements (arrays of measurements)
	float chi2(const float* dz, const float* dzend,
		const float* w, const float* x) const noexcept;

	/// combine two independent SIMDLines, return result
	SIMDLine combine(const SIMDLine& other) const noexcept;
};

/** @brief SIMDLineFit represents a straight line fit
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2016-01-06
 *
 * A simple straight line fit. Emphasis is put on SIMDised implemenation of
 * the most important routines. Measurements can be added one at a time, or in
 * vectors of four, or as SOA-style arrays (one array/pointer pair per
 * quantity).
 *
 * Use of the routines should be fairly self-explanatory.
 *
 * @note: There is some duplication of information going on in the data
 * members to make the data layout compatible with SIMD operations. Users will
 * not usually see this, but it's a fair point to mention.
 *
 * Example:
 * @code
 * const float dz[12] = { ... }; // 12 measurements: dz_i = z_i - z_ref
 * const float w[12] = { ... };  // 12 measurements: w_i = 1 / sigma_x_i
 * const float x[12] = { ... };  // 12 measurements: x_i
 * SIMDLineFit fit;
 * fit.addHits(dz, &dz[12], w, x);
 * SIMDLine line = fit.solve();
 * if (!line.ok()) {
 *     std::cout << "Line fit failed!" << std::endl;
 * } else {
 *     std::cout << "Line fit: x=" << line.x() << " tx=" << line.tx() <<
 *         " chi^2=" << line.chi2(dz, &dz[12], w, x) << std::endl;
 * }
 * @endcode
 */
class SIMDLineFit : public SIMDLineBase {
    private:
	/// accumulators ({ m00, m01, m10, m11 }, { rhs0, rhs1, rhs0, rhs1 })
	FloatVec m_par[2];

    public:
	/** @brief initialise a straight line fit
	 */
	SIMDLineFit(float m0 = 0.f, float m1 = 0.f, float m2 = 0.f,
		float rhs0 = 0.f, float rhs1 = 0.f) noexcept :
	    m_par{ FloatVec{ m0, m1, m1, m2 },
		    FloatVec{rhs0, rhs1, rhs0, rhs1} }
        { }

	/// do straigt line fit
	SIMDLineFit(const float* dz, const float* dzend,
		const float* w, const float* x) noexcept :
	    m_par{ FloatVec{ 0.f, 0.f, 0.f, 0.f },
		FloatVec{ 0.f, 0.f, 0.f, 0.f } }
	{ addHits(dz, dzend, w, x); }

	/// solve the linear system, and return the solution
	SIMDLine solve() const noexcept
	{
	    // 28 VFLOPS
	    const auto C = invert(m_par[0]);
	    const auto p = C * m_par[1];
	    return SIMDLine(p + simd_shuffle(p, 1, 0, 3, 2), C);
	}

	/// add a single hit (dz = z - z_ref, w = 1 / sigma_x, x = coordinate)
	void addHit(float dz, float w, float x) noexcept
        {
	    // 2 FLOPS, 5 VFLOPS, 1 VINIT
	    const FloatVec wdz{ w, w, w * dz, w * dz };
	    const auto wdzT = simd_shuffle(wdz, 0, 2, 1, 3);
	    m_par[0] += wdz * wdzT,
		m_par[1] += FloatVec{w * x, w * x, w * x, w * x} * wdzT;
	}

        /// add four hits (dz = z - z_ref, w = 1 / sigma_x, x = coordinate)
        void addHits(FloatVec dz, FloatVec w, FloatVec x) noexcept
        {
	    // 32 VFLOPS
	    const auto wdz = w * dz, wx = w * x;
	    m_par[0] += simd_shuffle2(hadd(w * w),
		    simd_shuffle2(hadd(w * wdz), hadd(wdz * wdz), 0, 1, 2, 7),
		    0, 5, 6, 7);
	    m_par[1] += simd_shuffle2(hadd(w * wx), hadd(wdz * wx), 0, 5, 2, 7);
	}

	/// add array of hits (dz = z - z_ref, w = 1 / sigma_x, x = coordinate)
	inline void addHits_aligned(const FloatVec* dz, const FloatVec* dzend,
		const FloatVec* w, const FloatVec* x) noexcept
	{
	    // 8 VFLOPS per added vector element, 32 per vector
	    while (dzend != dz) addHits(*dz++, *w++, *x++);
	}

	/// add array of hits (dz = z - z_ref, w = 1 / sigma_x, x = coordinate)
	void addHits(const float* dz, const float* dzend,
		const float* w, const float* x) noexcept;
};

// make sure alignment constraints can be met for SIMDLine
static_assert(sizeof(SIMDLine) == sizeof(SIMDLineBase::FloatVec[2]),
	"SIMDLineBase or SIMDLine have acquired extra data members!");
// make sure alignment constraints can be met for SIMDLineFit
static_assert(sizeof(SIMDLineFit) == sizeof(SIMDLineBase::FloatVec[2]),
	"SIMDLineBase or SIMDLineFit have acquired extra data members!");

#undef simd_shuffle
#undef simd_shuffle2

#endif // SIMDLINE_H

// vim: ft=cpp:sw=4:tw=78
