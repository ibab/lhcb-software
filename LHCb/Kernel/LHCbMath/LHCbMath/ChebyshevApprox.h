/** @file ChebyshevApprox.h
 *
 * Chebyshev approximation toolkit
 *
 * This file contains a simple implementation of routines determining
 * truncated Chebyshev expansions to some compile-time fixed order.
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-01-31
 */

#ifndef CHEBYSHEVAPPROX_H
#define CHEBYSHEVAPPROX_H

#include <array>
#include <cmath>
#include <limits>
#include <cassert>
#include <algorithm>
#include <type_traits>

/// implementation details for ChebyshevApprox
namespace ChebyshevApproxImpl
{
    /** @brief a very naive implementation of Chebyshev polynomials of the
     * first kind.
     *
     * @param n	n-th Chebyshev polynomial
     * @param x	point in [-1, 1] in which to evaluate it
     *
     * @returns 	value of @f$T_n(x)@f$
     *
     * @note This routine is slow for large n, and some better
     * implementation should be found.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-01-31
     */
    template <class FLT>
    FLT T(unsigned n, FLT x) noexcept
    {
	if (2 > n) return n ? x : 1;
	x *= 2;
	FLT u = 1, uu = 0;
	while (--n) {
	    uu = x * u - uu; // be FMA friendly!
	    std::swap(u, uu);
	}
	return (x * u) / 2 - uu;
    }

    /** @brief get coefficient j of Chebyshev expansion of f.
     *
     * @tparam NMAXPLUSONE
     * 		number of sampling points on [-1, 1]
     * @tparam FLT	(floating point) type for calculations
     *
     * @param j		number of coefficient to get
     * @param costbl	cos(k * PI / NMAX) tabulated (k=0, ..., NMAX)
     * @param ftbl		tabulated f(costbl[k]) (k=0, ..., NMAX)
     *
     * @returns	j-th coefficient of Chebyshev expansion of f
     *
     * exploits orthogonality relation of Chebyshev polynomials:
     * @f[
     * {\sum_{k=0}^{n}}'' T_i(x_k) T_j(x_k) =
     * \frac{n\delta_{ij}}{1+\delta_{0i}+\delta_{ni}}
     * @f]
     * where n = NMAX, the @f$T_i(x)@f$ are the Chebyshev polynomials of
     * the first kind, @f$x_k=\cos(\frac{k\pi}{n})@f, and the double prime
     * means that the first and last term of the sum need to be halved.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-01-31
     */
    template<size_t NMAXPLUSONE, class FLT>
    FLT getcoeff(unsigned j,
	    const std::array<FLT, NMAXPLUSONE>& costbl,
	    const std::array<FLT, NMAXPLUSONE>& ftbl) noexcept
    {
	constexpr size_t NMAX = NMAXPLUSONE - 1u;
	assert(j <= NMAX);
	// sum up
	auto itf = std::begin(ftbl) + 1;
	const FLT sum = std::accumulate(&costbl[1], &costbl[NMAX],
		// start and end point of interval, don't forget to halve
		(ftbl[NMAX] * T(j, FLT(-1)) + ftbl[0] * T(j, FLT(+1))) / 2,
		[&itf, j] (FLT psum, FLT x) // accumulate other terms
		{ return *itf++ * T(j, x) + psum; }); // be FMA friendly!
	// normalise according to Chebyshev orthogonality relation
	return (1 + (0 != j && NMAX != j)) * sum / NMAX;
    }

    /** @brief find x in [a, b] such that f(x) = y by bisection.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-01-31
     *
     * @param a		start of interval which brackets y
     * @param b 	end of interval which brackets y
     * @param fn	function for which we search x such that fn(x) = y
     * @param fmin	do not set, used internally
     * @param fmax	do not set, used internally
     * @param nmax	do not set, used internally
     *
     * @returns x, or NaN in case of failure
     *
     * Pick a and b such that they bracket y, i.e. fn(a) <= y <= fn(b) or
     * fn(b) <= y <= fn(a).
     */
    template<class FLT, class FN>
    constexpr static FLT bisect(FLT a, FLT b, FLT y, const FN& fn,
	    FLT fmin = 0, FLT fmax = 0, unsigned nmax = 100) noexcept
    {
	return 
	    // remember largest and smallest function values
	    (fmin == fmax) ? bisect(a, b, y, fn, fn(a), fn(b), nmax) :
	    // give limits a and b such that f(b) > f(a)
	    (fn(a) > fn(b)) ? bisect(b, a, y, fn, fmax, fmin, nmax) :
	    // if y is outside [f(a), f(b)], return a or b
	    (y <= fn(a)) ? a : (y >= fn(b)) ? b :
	    // make sure f(a) <= y <= f(b), and check for terminal
	    // non-convergence
	    ((fn(a) - y) * (fn(b) - y) > 0 || !nmax) ?
	    std::numeric_limits<FLT>::quiet_NaN() : // signal error if not
	    // check for convergence - have to be better than the 3 least
	    // significant bits of the mantissa in the result, or in the
	    // absolute or relative agreement of the function values that
	    // bracket the zero
	    ((fn(b) - fn(a)) < 8 * std::numeric_limits<FLT>::epsilon() ||
	     (fn(b) - fn(a)) / (fmax - fmin) <
	     8 * std::numeric_limits<FLT>::epsilon() ||
	     std::abs(b - a) < 8 * std::numeric_limits<FLT>::epsilon()) ?
	    ((b + a) / 2) : // return average if converged
	    // bisect, and proceed to interval of f(x) - y that has a zero
	    ((fn(a) - y)*(fn((b + a) / 2) - y) <= 0) ?
	    bisect(a, (b + a) / 2, y, fn, fmin, fmax, nmax - 1) :
	    bisect((b + a) / 2, b, y, fn, fmin, fmax, nmax - 1);
    }

    /** @brief helper to unroll Chebyshev expansion evaluation.
     *
     * This uses Clenshaw's algorithm to quickly evaluate the expansion
     * by iterating the recurrence relation @f$u_k = 2 x u_{k + 1} -
     * u_{k + 2} + c_k@f$ for k = N - 1, ..., 0; the recurrence is started
     * with @f$u_N = c_N, u_{N + 1} = 0@f$, and the result of the
     * calculation is @f$(u_0 - u_2 + c_0)/2@f$.
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-01-31
     *
     * @note The recurrence is slightly rearranged compared to the simple
     * version above, as this allows each step to be accomplished with one
     * fused multiply-and-add and a subtraction which will be beneficial
     * on the latest processors. DO NOT USE std::fma HERE - the compiler
     * might not inline it and call a subroutine if your hardware doesn't
     * support FMA, and you want to avoid the call at all costs; if your
     * CPU does, and the compiler knows your targeting a CPU with FMA
     * support, it will typically be smart enough to use the FMA
     * instructions (recent gcc versions are).
     */
    template <class FLT, unsigned N>
    struct eval_helper {
	constexpr FLT operator()(const FLT* c, const FLT twox,
		const FLT u, const FLT uu) const noexcept
	{ return eval_helper<FLT, N - 1>()( c, twox,
		twox * u - uu, // be FMA friendly!
		u - c[N - 1]); }
    };

    /// specialisation to terminate recurrence relation
    template <class FLT>
    struct eval_helper<FLT, 0u> {
	constexpr FLT operator()(const FLT*, const FLT twox,
		const FLT u, const FLT uu) const noexcept
	{ return (twox * u) / 2 - uu; }
    };

    /// specialisation: special case of linear approximation
    template <class FLT>
    struct eval_helper<FLT, -1u> {
	constexpr FLT operator()(const FLT*c, const FLT twox,
		const FLT, const FLT) const noexcept
	{ return c[1] * (twox / 2) + c[0]; }
    };

    /// specialisation: special case of constant approximation
    template <class FLT>
    struct eval_helper<FLT, -2u> {
	constexpr FLT operator()(const FLT*c, const FLT,
		const FLT, const FLT) const noexcept
	{ return *c; }
    };
}

/** @brief fixed order Chebyshev approximation of arbitrary functions
 *
 * @author Manuel Schiller <Manuel.Schiller@cern.ch>
 * @date 2015-01-31
 *
 * This class calculates a Chebyshev expansion of a given function over an
 * interval [a, b] to some compile-time fixed order NCOEFF, and allows for
 * fast and efficient evaluation of the resulting approximation. The class
 * derives an estimate of the approximation error over the given interval, and
 * is usually accurate in its estimates at the percent level.
 *
 * @tparam FLT		(floating point) type used for storing coefficients
 * 			and evaluating the result
 * @tparam NCOEFF	number of coefficients in approximation
 *
 * @note The number of coefficients, NCOEFF, is a compile time constant on
 * purpose; this allows the code to be structured such that the evaluation of
 * the approximation can be done fully unrolled, i.e. without any conditional
 * branches.
 */
template <class FLT, unsigned NCOEFF>
class ChebyshevApprox
{
    protected:
	FLT m_scale;	///< needed for transforming [a, b] to [-1, 1]
	FLT m_offset;	///< needed for transforming [a, b] to [-1, 1]
	std::array<FLT, NCOEFF> m_coeffs; ///< Chebyshev coefficients
	FLT m_errest;	///< error estimate

	/// default constructor
        ChebyshevApprox()
	{
	    // enforce sensible range of N
	    static_assert(0 < NCOEFF, "NCOEFF must be positive");
	}

	// all other ChebyshevApprox instantiations are our friends
	template <class FFLT, unsigned FNCOEFF> friend class ChebyshevApprox;

    public:
	/** @brief initialise truncated Chebyshev approximation with NCOEFF
	 * coefficients in interval [a, b] from given coefficients.
	 *
	 * @param a		start of range over which to sample f
	 * @param b		end of range over which to sample f
	 * @param coeffs	initializer_list of coefficients
	 * @param errest	error estimate to report
	 *
	 * @tparam IFLT	type for intermediate calculations (can be used to
	 * 		calculate intermediate results in double, but storing
	 * 		the coefficients as floats
	 */
	template<class IFLT>
	ChebyshevApprox(IFLT a, IFLT b, std::initializer_list<FLT> coeffs,
		FLT errest = std::numeric_limits<FLT>::max()) :
	    m_scale(IFLT(2) / (b - a)), m_offset(-(b + a) / (b - a)),
	    m_coeffs(coeffs), m_errest(errest)
        {
	    static_assert(NCOEFF == coeffs.size(),
		    "number of coeffs must match NCOEFF");
	    // also enforce sensible range of N
	    static_assert(0 < NCOEFF, "NCOEFF must be positive");
       	}

	/** @brief find truncated Chebyshev approximation with NCOEFF
	 * coefficients to function fn in interval [a, b].
	 *
	 * @param a	start of range over which to sample fn
	 * @param b	end of range over which to sample fn
	 * @param fn	callable object representing function
	 *
	 * @tparam FN	type of callable entity ("function") to approximate
	 * @tparam IFLT	type for intermediate calculations (can be used to
	 * 		calculate intermediate results in double, but storing
	 * 		the coefficients as floats
	 * @tparam NSAMPLES
	 * 		number of points in which to sample when calculating
	 * 		coefficients (or, in other words, produce an
	 * 		approximation of order NSAMPLES, and truncate it after
	 * 		the first NCOEFF coefficients)
	 */
	template<class FN, class IFLT, unsigned NSAMPLES = 128>
	ChebyshevApprox(IFLT a, IFLT b, const FN fn) :
	    m_scale(IFLT(2) / (b - a)), m_offset(-(b + a) / (b - a)),
	    m_errest(0)
	{
	    static_assert(std::is_same<IFLT, decltype(fn(a))>::value,
		    "return type of f doesn't match type IFLT");
	    // also enforce sensible range of N
	    static_assert(0 < NCOEFF, "NCOEFF must be positive");
	    // need some headroom in NSAMPLES to produce error estimate
	    static_assert(NCOEFF + 10 <= NSAMPLES,
		    "NCOEFF too large, increase NSAMPLES");
	    assert(a < b);
	    // keep tables of cos(k PI / NSAMPLES) and f(cos(k PI / NSAMPLES))
	    // for getcoeff's use, see below
	    std::array<IFLT, NSAMPLES + 1> costbl, ftbl;
	    // fill costbl with cos(0), cos(pi / n) ... cos((n - 1) * pi / n),
	    // cos(pi)
	    {
		// don't generate a gazillion cosines from scratch, but use
		// Chebyshev-based recursion relation below to save some work:
		// cos(nx) = 2 * cos(x) * cos((n - 1) x) - cos((n - 2) x)
		const IFLT c = 2 * std::cos(IFLT(M_PI) / NSAMPLES);
		IFLT cc = c / 2, oc = 1;
		costbl[0] = 1;
		std::generate(&costbl[1], &costbl[NSAMPLES], [c, &cc, &oc] ()
			{ oc = c * cc - oc; // be FMA friendly!
			std::swap(cc, oc); return oc; });
		costbl[NSAMPLES] = -1;
	    }
	    // tabulate function in the corresponding points
	    {
		// construct adapter to fn which takes arguments from [-1, 1]
		// instead of [a, b]
		const IFLT im = (b + a) / 2, id = (b - a) / 2;
		const auto fnprime = [im, id, &fn] (IFLT x) {
		    return fn(id * x + im); }; // be FMA friendly
		std::transform(std::begin(costbl), std::end(costbl),
			std::begin(ftbl), [&fnprime] (IFLT x) {
			return fnprime(x); });
	    }
	    /// generate the first N coefficients, and place them into m_coeffs
	    unsigned i = 0;
	    std::generate(std::begin(m_coeffs), std::end(m_coeffs),
		    [&i, &costbl, &ftbl] () {
		    return ChebyshevApproxImpl::getcoeff(i++, costbl, ftbl); });
	    // get error estimate: start with first neglected term, and keep
	    // summing up until the increase gets below a percent of the initial
	    // estimate (note i == N here)
	    FLT derrest;
	    do {
		derrest = std::abs(ChebyshevApproxImpl::getcoeff(
			    i++, costbl, ftbl));
		m_errest += derrest;
	    } while (i < NSAMPLES && // sum only finite number of terms
		    0 < m_errest && // stop if they're all zero, or if they
		    (128 * derrest) > m_errest); // don't change much any more
	}

	/** @brief find truncated Chebyshev approximation c(y) with NCOEFF
	 * coefficients which statisfies c(y) = f(x(y)) for all x in [a, b].
	 *
	 * y(x) must be a monotonic function of x (otherwise, the solution
	 * isn't unique, and the algorithm will fail in undefined ways).
	 *
	 * @param a	start of range over which to sample f
	 * @param b	end of range over which to sample f
	 * @param f	function which to approximate as function of y
	 * @param y	calculate y(x) for x in [a, b]
	 *
	 * @returns Chebyshev approximation of f(x(y))
	 *
	 * @tparam FN	type of callable entity ("function") to approximate
	 * @tparam FN2	type of callable entity ("function") which gives y(x)
	 * @tparam IFLT	type for intermediate calculations (can be used to
	 * 		calculate intermediate results in double, but storing
	 * 		the coefficients as floats
	 * @tparam NSAMPLES
	 * 		number of points in which to sample when calculating
	 * 		coefficients (or, in other words, produce an
	 * 		approximation of order NSAMPLES, and truncate it after
	 * 		the first NCOEFF coefficients)
	 */
	template<class FN, class FN2, class IFLT, unsigned NSAMPLES = 128>
	static ChebyshevApprox<FLT, NCOEFF> implicit(
		IFLT a, IFLT b, const FN f, const FN2 y)
	{
	    static_assert(std::is_same<IFLT, decltype(y(a))>::value,
		    "return type of y doesn't match type IFLT");
	    return ChebyshevApprox(y(a), y(b), [a, b, &f, &y] (IFLT yy) {
		    return f(ChebyshevApproxImpl::bisect(a, b, yy, y)); });
	}


	/** @brief find truncated Chebyshev approximation with NCOEFF
	 * coefficients to the inverse of function fn, i.e. find c(y)
	 * such that y = fn(c(y)) for all y in [fn(a), fn(b)].
	 *
	 * fn(x) must be a monotonic function of x (otherwise, the solution
	 * isn't unique, and the algorithm will fail in undefined ways).
	 *
	 * @param a	start of range over which to sample fn
	 * @param b	end of range over which to sample fn
	 * @param fn	callable object representing function
	 *
	 * @tparam FN	type of callable entity ("function") to approximate
	 * @tparam IFLT	type for intermediate calculations (can be used to
	 * 		calculate intermediate results in double, but storing
	 * 		the coefficients as floats
	 * @tparam NSAMPLES
	 * 		number of points in which to sample when calculating
	 * 		coefficients (or, in other words, produce an
	 * 		approximation of order NSAMPLES, and truncate it after
	 * 		the first NCOEFF coefficients)
	 */
	template<class FN, class IFLT, unsigned NSAMPLES = 128>
	static ChebyshevApprox<FLT, NCOEFF> inverse(IFLT a, IFLT b, const FN fn)
	{ return implicit(a, b, [] (IFLT x) { return x; }, fn); }

	/** @brief evaluate Chebyshev expansion
	 *
	 * This routine calculates @f$\sum_{k=0}^{N-1} c_k T_k(x)@f$.
	 *
	 * @param x point in which to evaluate expansion
	 *
	 * @returns  value of expansion at point x
	 */
	constexpr inline FLT operator()(const FLT x) const noexcept
	{
	    static_assert(0 < NCOEFF, "positive number of coefficients needed");
	    return
		ChebyshevApproxImpl::eval_helper<FLT, NCOEFF - 2>()(&m_coeffs[0],
			2 * (m_scale * x + m_offset), // be FMA friendly
			m_coeffs[NCOEFF - 1],
			((NCOEFF > 1) ? -m_coeffs[NCOEFF - 2] : FLT(0)));
	}
	
	/** @brief return an estimate of the truncation error.
	 *
	 * @returns estimate of the truncation error of the approximation
	 */
	constexpr inline FLT error_estimate() const noexcept
	{ return m_errest; }

	/** @brief set error estimate, e.g. after tweaking coefficients.
	 *
	 * @param errest	new error estimate
	 */
	void set_error_estimate(FLT errest)
	{ m_errest = errest; }

	/** @brief get number of coefficients. **/
	constexpr inline unsigned size() const noexcept
	{ return m_coeffs.size(); }

	/** @brief get coefficient i of Chebyshev expasion. */
	constexpr inline FLT operator[](unsigned i) const noexcept
	{ return m_coeffs[i]; }

	/** @brief read-write access to coefficient i of Chebyshev expasion. */
	inline FLT& operator[](unsigned i) noexcept
	{ return m_coeffs[i]; }

	/** @brief return derivative of this ChebyshevApprox
	 *
	 * Uses the known Chebyshev coefficients of this approximation to
	 * analytically calculate its derivative as Chebyshev expansion.
	 *
	 * The error estimate is done on a best guess basis, and should be
	 * considered to be only an indication.
	 */
	inline ChebyshevApprox<FLT,
	       (1 == NCOEFF) ? 1 : (NCOEFF - 1)> derivative() const noexcept
	{
	    ChebyshevApprox<FLT, (1 == NCOEFF) ? 1 : (NCOEFF - 1)> retVal;
	    retVal.m_scale = m_scale, retVal.m_offset = m_offset;
	    if (NCOEFF < 3) {
		retVal[0] = (1 == NCOEFF) ? 0 : m_coeffs[1];
	    } else {
		retVal[NCOEFF - 2] = 2 * (NCOEFF - 1) * m_coeffs[NCOEFF - 1];
		for (unsigned k = NCOEFF - 2; k; --k)
		    retVal[k - 1] = (2 * k) * m_coeffs[k] + retVal[k + 1];
		// retVal[0] needs adjusting
		retVal[0] -= m_coeffs[1];
		for (auto& c: retVal.m_coeffs) c *= m_scale;
	    }
	    // we have @f$(\epsilon T_n)'=\epsilon n U_{n-1}@f$, and the
	    // absolute value of the maximum of @f$U_{n-1}@f$ on [-1, 1] is
	    // n, so we estimate the error to be @f$n^2\epsilon@f$
	    retVal.m_errest = m_errest * std::pow(NCOEFF, 2);
	    return retVal;
	}

	/** @brief return integral of this ChebyshevApprox
	 *
	 * Uses the known Chebyshev coefficients of this approximation to
	 * analytically calculate its integral as Chebyshev expansion.
	 *
	 * @param c	constant of integration at lower edge of interval
	 */
	inline ChebyshevApprox<FLT, NCOEFF + 1> integral(
		FLT c = 0) const noexcept
	{
	    ChebyshevApprox<FLT, NCOEFF + 1> retVal;
	    retVal.m_scale = m_scale, retVal.m_offset = m_offset;
	    retVal[0] = 0;
	    const FLT sc = 1 / (2 * m_scale);
	    unsigned k;
	    for (k = 1; k < NCOEFF - 1; ++k) {
		retVal[k] = (sc / k) * (m_coeffs[k - 1] - m_coeffs[k + 1]);
	    }
	    retVal[k] = (sc / k) * m_coeffs[k - 1];
	    ++k;
	    retVal[k] = (sc / k) * m_coeffs[k - 1];
	    // m_coeffs[0] needs adjusting
	    retVal[1] += sc * m_coeffs[0];
	    // adjust constant of integration to give c at I(a)
	    retVal[0] = c -
		ChebyshevApproxImpl::eval_helper<FLT, NCOEFF - 1>()(
			&retVal.m_coeffs[0], FLT(-2),
			retVal[NCOEFF], -retVal[NCOEFF - 1]);
	    // error estimate becomes (b - a) * m_errest / (NCOEFF + 1)
	    retVal.m_errest = 2 * m_errest / (NCOEFF + 1) / m_scale;
	    return retVal;
	}
};

#endif // CHEBYSHEVAPPROX_H

// vim: sw=4:tw=78:ft=cpp
