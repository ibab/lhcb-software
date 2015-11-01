/** @file SIMDChi2Fit.h
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 */
#ifndef SIMDCHI2FIT_H
#define SIMDCHI2FIT_H

namespace std {
    // force gcc and friends to inline inner_product aggressively
#if defined(__GNUC__)
    template <class InputIt1, class InputIt2, class T>
    T inner_product(InputIt1 first1, InputIt1 last1,
	    InputIt2 first2, T value) __attribute__((
		    hot,
		    optimize("-ffast-math", "-ftree-vectorize",
		       	"-fvect-cost-model", "-ftree-slp-vectorize",
		       	"-ftree-vect-loop-version", "-funroll-all-loops"),
		    always_inline));
#endif
}

#include <cmath>
#include <array>
#include <cassert>
#include <utility>
#include <algorithm>
#include <functional>
#include <type_traits>

#include "Math/CholeskyDecomp.h"
#include "GaudiKernel/Kernel.h" // LIKELY/UNLIKELY

namespace Tf {
/// SIMDChi2FitUtils - autovectorisable @f$\chi^2@f$ fits
namespace SIMDChi2FitUtils {

/** @brief policy object for SIMDChi2Fit to define basic parameters
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 *
 * @tparam FLOATTYPE	floating point type for calculations
 * @tparam TRACKTYPE	track (data) type
 * @tparam HITTYPE	hit (data) type
 * @tparam NDIM		dimensionality of the parameter space
 * @tparam NITERMAX	how often to iterate the fit
 * @tparam DEFERREDCOVARIANCE
 * 			if the object should support returning the covariance
 * 			matrix
 */
template <class FLOATTYPE, class TRACKTYPE, class HITTYPE,
	 unsigned NDIM, unsigned NITERMAX, bool DEFERREDCOVARIANCE = false>
struct CommonFitPolicy
{
    enum {
	nDim = NDIM ///< dimensionality of fit
    };
    enum {
	nIterMax = NITERMAX ///< how often to iterate the fit
    };
    enum {
	/// support returning the covariance matrix
	deferredCovariance = DEFERREDCOVARIANCE
    };
    /// possible states after updating hit parameters
    typedef enum {
	NotPosDef,	///< lin. system not pos. definite
	UpdateRejected,	///< fit policy rejects outrageous parameter update
	UpdateAccepted,	///< fit policy accepts parameter update
	FitConverged	///< fit policy has found convergence
    } FitStatus;

    /// number representation
    typedef FLOATTYPE value_type;
    /// Track class
    typedef TRACKTYPE track_type;
    /// Hit class
    typedef HITTYPE hit_type;
    /// vector type (for gradient)
    typedef std::array<value_type, nDim> vector_type;
    /// (packed) matrix type (cov. matrix)
    typedef std::array<value_type, (nDim * (nDim + 1)) / 2> matrix_type;
    
    /// export our own type to derived classes
    typedef CommonFitPolicy<FLOATTYPE, TRACKTYPE, HITTYPE,
	    NDIM, NITERMAX, DEFERREDCOVARIANCE>
	commonfitpolicy_type;

    // protect against user error
    static_assert(nDim > 0,
	    "Fit parameter space must have positive dimension.");
    static_assert(nIterMax > 0,
	    "Maximum number of fit iterations must be positive.");
    static_assert(std::is_floating_point<value_type>::value,
	    "Invalid type for value_type - must be floating point type.");
    static_assert(std::is_class<track_type>::value ||
	    std::is_reference<track_type>::value ||
	    std::is_pointer<track_type>::value,
	    "Invalid type for track_type - must be of class/struct, "
	    "reference or pointer type.");
    static_assert(std::is_class<hit_type>::value ||
	    std::is_reference<hit_type>::value ||
	    std::is_pointer<hit_type>::value,
	    "Invalid type for hit_type - must be of class/struct, "
	    "reference or pointer type.");

#define COMMONPOLICY_INJECT_TYPES(COMMONPOLICY) \
    using value_type = typename COMMONPOLICY::value_type; \
    using track_type = typename COMMONPOLICY::track_type; \
    using hit_type = typename COMMONPOLICY::hit_type; \
    using vector_type = typename COMMONPOLICY::vector_type; \
    using matrix_type = typename COMMONPOLICY::matrix_type; \
    using commonfitpolicy_type = typename COMMONPOLICY::commonfitpolicy_type; \
    using FitStatus = typename COMMONPOLICY::FitStatus; \
    using COMMONPOLICY::nDim; \
    using COMMONPOLICY::nIterMax; \
    using COMMONPOLICY::deferredCovariance; \
    using COMMONPOLICY::NotPosDef; \
    using COMMONPOLICY::UpdateRejected; \
    using COMMONPOLICY::UpdateAccepted; \
    using COMMONPOLICY::FitConverged
};


/** @brief MeasurementProvider - policy class to get a measurement from a hit
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 *
 * This class tells the SIMDChi2Fit framework how to get the measured
 * coordinate from a hit. User code is supposed to implement a derived class to
 * implement the required functionality. It also provides the possibility to
 * "seed" the inverse covariance matrix and the right hand side of the linear
 * system solved in the fit with non-zero values.
 */
template <class COMMONPOLICY> struct MeasurementProvider : public COMMONPOLICY
{
    COMMONPOLICY_INJECT_TYPES(COMMONPOLICY);
    /// return if the hit is valid (for inclusion in the fit)
    bool valid(const hit_type& hit) const;
    /** @brief return weight times measurement ("x / sigma_x")
     *
     *
     * @tparam Proj	measurement projector class to be used
     *			(e.g. to resolve left-right ambiguities towards the
     *			wire, or to modify the measurement error depending on
     *			the angle of incidence)
     *
     * @param hit	hit for which to provide measurement
     * @param track	track (e.g. to resolve left/right ambiguities towards
     * 			the track in a drift tube detector)
     * @param projector	measurement projector instance to be used
     *			(e.g. to resolve left-right ambiguities towards the
     *			wire, or to modify the measurement error depending on
     *			the angle of incidence)
     *
     * @returns weighted measurement
     */
    template <class PROJ>
    value_type wmeas(const hit_type& hit, const track_type& track,
	    const PROJ& projector) const;
};

/** @brief MeasurementProjector - policy class to project a track into the
 * measurement space of a hit
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 *
 * This class tells the SIMDChi2Fit framework how to project the track
 * parameters into the measurement space of a hit. User code is supposed to
 * implement a derived class to implement the required functionality.
 */
template <class COMMONPOLICY> struct MeasurementProjector : public COMMONPOLICY
{
    COMMONPOLICY_INJECT_TYPES(COMMONPOLICY);
    /// return weight times measurement ("x(track) / sigma_x")
    value_type wproj(
	    const track_type& track, const hit_type& hit) const;
    /// return weight times gradient vector
    vector_type wgrad(
	    const track_type& track, const hit_type& hit) const;
    /** @brief return the initial right hand side of the linear system before
     * adding hits
     */
    vector_type seedRhs(const track_type& /*track*/) const
    { vector_type retVal; retVal.fill(value_type(0));  return retVal; }
    /** @brief return the initial inverse covariance matrix of the linear
     * system before adding hits
     */
    matrix_type seedICov(const track_type& /*track*/) const
    { matrix_type retVal; retVal.fill(value_type(0));  return retVal; }
};

/** @brief TrackUpdater - policy class to update a track with a track parameter
 * delta vector
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 *
 * This class tells the SIMDChi2Fit framework how to update the parameters of a
 * track. User code is supposed to implement a derived class to implement the
 * required functionality.
 */
template <class COMMONPOLICY> struct TrackUpdater : public COMMONPOLICY
{
    COMMONPOLICY_INJECT_TYPES(COMMONPOLICY);
    /// update track parameters, returns rejected, accepted, converged
    FitStatus updateTrack(
	    track_type& track, const vector_type& dparam) const;
};

/** @brief HitUpdater - policy class to update a hit following a track update
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-18
 *
 * This class tells the SIMDChi2Fit framework how to update a hit after a track
 * parameter update. User code is supposed to implement a derived class to
 * implement the required functionality.
 */
template <class COMMONPOLICY> struct HitUpdater : public COMMONPOLICY
{
    COMMONPOLICY_INJECT_TYPES(COMMONPOLICY);
    /// update track parameters, returns rejected, accepted, converged
    void updateHit(
	    hit_type& hit, const track_type& track) const;
};

/// SIMDChi2Fit implementation details
namespace SIMDChi2FitImplDetail {
    /// forward decls
    template <class M, size_t N>
    class transposer;
    template<class M, size_t N>
    class transposer_helper;

    /// smart class representing row of transposed view of matrix
    template<class M, size_t N>
    class transposer_helper
    {
	private:
	    M& m_lhs; ///< underlying matrix
	    size_t m_idx; ///< column of the underlying matrix to be written/read

	    friend class transposer<M, N>;
	    template<class MM, size_t NN>
	    friend class transposer_helper;

	    transposer_helper(M& lhs, size_t idx) noexcept :
		m_lhs(lhs), m_idx(idx)
	    { }

	public:
	    /// assign to this row
	    template<class T>
	    auto operator=(const T& rhs) const noexcept -> decltype(*this)
	    {
		transposer_helper<M, N - 1>(m_lhs, m_idx) = rhs;
		m_lhs[N - 1][m_idx] = rhs[N - 1];
		return *this;
	    }

	    /// read/write access to underlying matrix elements
	    auto operator[](
		    size_t idx) const noexcept -> decltype(m_lhs[idx][m_idx])&
	    { return m_lhs[idx][m_idx]; }
    };

    /// specialisation to terminate recursion
    template<class M>
    class transposer_helper<M, 0>
    {
	private:
	    friend class transposer<M, 0>;
	    template<class MM, size_t NN>
	    friend class transposer_helper;

	    transposer_helper(M&, size_t) noexcept { }

	public:
	    template<class T>
            auto operator=(const T&) const noexcept -> decltype(*this)
	    { return *this; }
    };

    /// transposed view of underlying matrix
    template <class M, size_t N>
    class transposer
    {
	private:
	    M& m_lhs; ///< underlying matrix

	public:
	    /// constructor
	    transposer(M& lhs) noexcept : m_lhs(lhs) { }

	    /// return view of column of underlying matrix as row vector
	    transposer_helper<M, N> operator[](size_t idx) const noexcept
	    { return transposer_helper<M, N>(m_lhs, idx); }
    };

    /// return a transposed view of given matrix
    template<size_t N, class M>
    transposer<M, N> make_transposed_view(M& lhs) noexcept
    { return transposer<M, N>(lhs); }
}

/// vectorisation support - implementation
namespace vect_impl {
#if defined(__GNUC__)
    /// dot product @f$\sum_i a_i * b_i@f$
    template <typename T, unsigned N>
    struct sdot {
	T operator()(const T* a, const T *b, T init)
	{
	    return (1 == N) ? (*a * *b + init) :
		sdot<T, N - N / 2>()(a + N / 2, b + N / 2,
			sdot<T, N / 2>()(a, b, init));
	}
    };

    /// vectorised dot product @f$\sum_i a_{Wi+k} * b_{Wi+k}@f$ (k=0, ..., W-1)
    template <typename T, unsigned N, unsigned W>
    struct vdot {
#ifdef __clang__
	typedef T __attribute__((ext_vector_type(W))) vT;
#else
	typedef T __attribute__((vector_size(W * sizeof(T)))) vT;
#endif
	vT operator()(const vT* a, const vT *b)
	{
	    return (1 == N) ? (*a * *b) :
		(vdot<T, N / 2, W>()(a, b) +
		 vdot<T, N - N / 2, W>()(a + N / 2, b + N / 2));
	}
    };

    /// dot product driver
    template <typename T, unsigned N, unsigned W>
    struct dot {
#ifdef __clang__
	typedef T __attribute__((ext_vector_type(W))) vT;
#else
	typedef T __attribute__((vector_size(W * sizeof(T)))) vT;
#endif
	T operator()(const T* a, const T* b, T init)
	{
	    if (N / W) {
		// we can vectorise at current width W, so do it
		auto tmp = vdot<T, N / W, W>()((const vT *) a, (const vT*) b);
		for (unsigned i = 0; W != i; ++i) init += tmp[i];
	    }
	    /// return, try doing remainder of dot product at half width
	    return (N - W * (N / W)) ?
		dot<T, N - W * (N / W), W / 2>()(
			a + W * (N / W), b + W * (N / W), init) : init;
	}
    };

    /// dot product kernel driver (specialised W = 1)
    template <typename T, unsigned N>
    struct dot<T, N, 1> {
	T operator()(const T* a, const T* b, T init)
	{ return sdot<T, N>()(a, b, init); }
    };
#endif
}

/// contains vectorised implementations of required routines
namespace vect {
#if defined(__GNUC__)
    /** @brief compute the dot product between two vectors
     *
     * @tparam T		data type of the underlying vectors
     * @tparam SIMDWIDTH	maximum SIMD width that the CPU can handle
     *
     * @param a		beginning of vector a
     * @param aend		end of vector a
     * @param b		beginning of vector b
     * @param init		initial value
     *
     * @returns @f$init + \sum_i a_i b_i @f$
     *
     * @author Manuel Schiller <Manuel.Schiller@cern.ch>
     * @date 2015-05-22
     *
     * @note Do not choose SIMDWIDTH larger than the combination of your CPU
     * and your data type T will support, or things will slow down!
     */
    template <typename T, unsigned SIMDWIDTH>
    T dot_product(const T* a, const T* aend, const T* b, T init)
    {
	using namespace vect_impl;
	// unroll loop up to length 32; use (individually optimised) fixed
	// length dot product kernels to compute the full product
	while (aend - a >= 32) { // 32 or more elements left
	    init = dot<T, 32, SIMDWIDTH>()(a, b, init);
	    a += 32, b += 32;
	}
	switch (aend - a) { // take care of the rest
	    case 31: return dot<T, 31, SIMDWIDTH>()(a, b, init);
	    case 30: return dot<T, 30, SIMDWIDTH>()(a, b, init);
	    case 29: return dot<T, 29, SIMDWIDTH>()(a, b, init);
	    case 28: return dot<T, 28, SIMDWIDTH>()(a, b, init);
	    case 27: return dot<T, 27, SIMDWIDTH>()(a, b, init);
	    case 26: return dot<T, 26, SIMDWIDTH>()(a, b, init);
	    case 25: return dot<T, 25, SIMDWIDTH>()(a, b, init);
	    case 24: return dot<T, 24, SIMDWIDTH>()(a, b, init);
	    case 23: return dot<T, 23, SIMDWIDTH>()(a, b, init);
	    case 22: return dot<T, 22, SIMDWIDTH>()(a, b, init);
	    case 21: return dot<T, 21, SIMDWIDTH>()(a, b, init);
	    case 20: return dot<T, 20, SIMDWIDTH>()(a, b, init);
	    case 19: return dot<T, 19, SIMDWIDTH>()(a, b, init);
	    case 18: return dot<T, 18, SIMDWIDTH>()(a, b, init);
	    case 17: return dot<T, 17, SIMDWIDTH>()(a, b, init);
	    case 16: return dot<T, 16, SIMDWIDTH>()(a, b, init);
	    case 15: return dot<T, 15, SIMDWIDTH>()(a, b, init);
	    case 14: return dot<T, 14, SIMDWIDTH>()(a, b, init);
	    case 13: return dot<T, 13, SIMDWIDTH>()(a, b, init);
	    case 12: return dot<T, 12, SIMDWIDTH>()(a, b, init);
	    case 11: return dot<T, 11, SIMDWIDTH>()(a, b, init);
	    case 10: return dot<T, 10, SIMDWIDTH>()(a, b, init);
	    case  9: return dot<T,  9, SIMDWIDTH>()(a, b, init);
	    case  8: return dot<T,  8, SIMDWIDTH>()(a, b, init);
	    case  7: return dot<T,  7, SIMDWIDTH>()(a, b, init);
	    case  6: return dot<T,  6, SIMDWIDTH>()(a, b, init);
	    case  5: return dot<T,  5, SIMDWIDTH>()(a, b, init);
	    case  4: return dot<T,  4, SIMDWIDTH>()(a, b, init);
	    case  3: return dot<T,  3, SIMDWIDTH>()(a, b, init);
	    case  2: return dot<T,  2, SIMDWIDTH>()(a, b, init);
	    case  1: return dot<T,  1, SIMDWIDTH>()(a, b, init);
	    default: break;
	}
	return init;
    }
#endif
}

/// @brief perform a simple iterative @f$\chi^2@f$ fit that can be
/// autovectorised
///
/// @author Manuel Schiller <manuel.schiller@nikhef.nl>
/// @date 2014-08-18
///
/// The following template parameters allow the behaviour of the fitter to be
/// customised:
///
/// @tparam MEASUREMENTPROVIDER
/// 	class providing @f$x_k/\sigma_k@f$ for each hit; it will also return a
/// 	flag specifying if the hit should be included in the fit
///
/// @tparam MEASUREMENTPROJECTOR
/// 	class providing @f$f(z_k; \vec{p})/\sigma_k@f$ and
/// 	@f$\vec{\nabla}_{\vec{p}} f(z_k; \vec{p})/\sigma_k@f$ given a track and
/// 	a hit
/// 
/// @tparam TRACKUPDATER
/// 	class which knows how to apply a track parameter update
/// 	@f$\delta\vec{p}@f$ to a track
///
/// @tparam HITUPDATER
/// 	class which is knows how to update hits after a track parameter update
/// 	(can do nothing if no such action is required)
///
/// @tparam NHITSMAX
/// 	maximum number of hits to add to the inverse covariance matrix using
/// 	auto-vectorised loops in one go (needs an array on the stack, so this
/// 	shouldn't be larger than the number of hits you expect on a track)
/// 	
/// @tparam SIMDWIDTH
/// 	width of the SIMD path on the architecture used, if the CPU can operate
/// 	on four doubles with a single instruction, this should be 4 (if this
/// 	value is too large, it incurs some tiny inefficiency, but it should be
/// 	a multiple of the true SIMD path width)
///
/// The template parameters MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
/// TRACKUPDATER and HITUPDATER all inherit from the templated CommonFitPolicy
/// class which allows user code to chose things like:
/// - floating point type used in calculations
/// - dimensionality of the fit parameter space
/// - track and hit type
/// - how many times to iterate the fit before giving up
///
/// @note At "-O2", the auto-vectorisation needs to be triggered with compiler
/// flags "-ffast-math -ftree-vectorize", "-fvect-cost-model"; of course, the more CPU features (SSE,
/// SSE2, SSE3, SSE4, AVX, ...) you allow the compiler to take advantage of, the
/// better it gets.
///
/// @section details Details
/// This class performs a @f$\chi^2@f$ fit where @f$\chi^2@f$ is given by
/// @f[ \chi^2 = \sum_k \left(
///     \frac{x_k - f(z_k; \vec{p}+\delta \vec{p})}{\sigma_k}
/// \right)^2 @f]
/// where @f$x_k@f$ is the measurement of the @f$k@f$-th hit at coordinate
/// @f$z_k@f$, and @f$f(z_k;\vec{p}+\delta \vec{p})@f$ is the projection
/// function that projects the track parameters into the measurement space of
/// the hit (or, in other words, calculates the coordinate the hit should have
/// based on track parameters). @f$f(z_k;\vec{p}+\delta \vec{p})@f$ is
/// explicitly written in a form that is easily expanded in small track
/// parameter shifts @f$\delta\vec{p}@f$ around a reasonable estimate
/// @f$\vec{p}@f$ of track parameters to permit solving the problem iteratively
/// for small non-linearities in @f$f@f$.
/// 
/// To minimise the @f$\chi^2@f$, the gradient is put to zero:
/// @f[ \vec{0} = \vec{\nabla}_{\delta\vec{p}} \chi^2 =
/// 2 \sum_k \frac{1}{\sigma_k^2} \left(
/// x_k - f(z_k; \vec{p}+\delta\vec{p}) \right) \cdot
/// \vec{\nabla}_{\delta\vec{p}} f(z_k; \vec{p}+\delta\vec{p})
/// @f]
/// Taylor-expanding around the current best-fit parameter guess @f$\vec{p}@f$
/// (and using @f$\vec{\nabla}_{\delta\vec{p}} f(z_k; \vec{p}+\delta\vec{p})
/// =\vec{\nabla}_{\vec{p}} f(z_k; \vec{p})@f$), one obtains:
/// @f[ \vec{0} = \sum_k \frac{1}{\sigma_k^2} \left(
/// x_k - f(z_k; \vec{p}) - \vec{\nabla}_{\vec{p}}f(z_k; \vec{p})\cdot
/// \delta\vec{p}) \right) \cdot
/// \vec{\nabla}_{\vec{p}} f(z_k; \vec{p}+\delta\vec{p})
/// @f]
/// Now one can trivially solve for @f$\delta\vec{p}@f$ by calculating
/// @f[
/// \sum_k \frac{1}{\sigma_k^2}
/// (\vec{\nabla}_{\vec{p}} f(z_k; \vec{p}))_i \cdot
/// (\vec{\nabla}_{\vec{p}} f(z_k; \vec{p}))_j \cdot (\delta\vec{p})_j = 
/// \sum_k \frac{1}{\sigma_k^2}
/// (\vec{\nabla}_{\vec{p}} f(z_k; \vec{p}))_i \cdot
/// (x_k - f(z_k; \vec{p}))
/// @f]
/// and iterate, updating the track parameters until @f$|\delta\vec{p}|@f$
/// becomes small enough. Care is taken in the class to structure the code such
/// that modern C++ compilers can auto-vectorise the sum over the hits @f$k@f$.
///
/// @section example Example
/// To give an example of how to use this class, suppose you have a simple Track
/// class for a parabolic track model, and a equally simple Hit class:
/// @code
/// struct Track {
///   double a, b, c;
///   double x(double z) const { return a + z * (b + z * c); }
/// };
/// struct Hit {
///     double x, z, sigma_x;
///     bool valid;
/// };
/// @endcode
/// To configure the fit, you would need the following code:
/// @code
/// // double precision, say which Track and Hit classes are to be used, 3 track
/// // parameters, max. 10 iterations
/// typedef CommonFitPolicy<double, Track, Hit, 3, 10> SimpleXFitPolicy;
///
/// // a MeasurementProvider class knows how to get the measurement from the hit
/// struct SimpleXMeasurementProvider :
///         public MeasurementProvider<SimpleXFitPolicy>
/// {
///    template <class PROJ>
///    value_type wmeas(const hit_type& hit,
///        const track_type& /* track_not_required_here */,
///        const PROJ& /* projector_not_required_here */) const
///    { return hit.x / hit.sigma_x; }
///    bool valid(const hit_type& hit) const
///    { return hit.valid; }
/// };
///
/// // a MeasurementProjector class knows how to "project" the track parameters
/// // into the measurement space of a hit
/// struct SimpleXMeasurementProjector :
///         public MeasurementProjector<SimpleXFitPolicy>
/// {
///     value_type wproj(const track_type& track, const hit_type& hit) const
///     { return track.x(hit.z) / hit.sigma_x; }
///     vector_type wgrad(const track_type& track, const hit_type& hit) const
///     {
///         return { { value_type(1) / hit.sigma_x, hit.z / hit.sigma_x,
///             hit.z * hit.z / hit.sigma_x } };
///     }
/// };
/// 
/// // the TrackUpdater knows how to modify the Track class on parameter
/// // updates; it also keeps a critical eye on the absolute size of the updates
/// // to tell if things converge or diverge
/// struct SimpleXTrackUpdater : public TrackUpdater<SimpleXFitPolicy>
/// {
///     FitStatus updateTrack(track_type& track, const vector_type& dparam) const
///     {
///         if (std::abs(dparam[0]) > 6e3 || std::abs(dparam[1]) > 3. ||
///                 std::abs(dparam[2]) > 1e-2)
///             return UpdateRejected;
///         // update track parameters
///         track.a += dparam[0];
///         track.b += dparam[1];
///         track.c += dparam[2];
///         if (std::abs(dparam[0]) < 1e-3 && std::abs(dparam[1]) < 1e-6 &&
///                 std::abs(dparam[2]) < 1e-9)
///             return FitConverged;
///         return UpdateAccepted;
///     }
/// };
/// 
/// // the HitUpdater updates hits after a track parameter update; in this
/// // example, it has nothing to do...
/// struct SimpleXHitUpdater : public HitUpdater<SimpleXFitPolicy>
/// { void updateHit(hit_type& hit, const track_type& track) const { } };
/// 
/// // finally: assemble the fully customised type for the parabola fit
/// typedef SIMDChi2Fit<SimpleXMeasurementProvider, SimpleXMeasurementProjector,
/// 	SimpleXTrackUpdater, SimpleXHitUpdater, 16> SimpleXFit;
/// @endcode
/// With this code in place, fitting a track becomes very easy indeed:
/// @code
/// Track track = //...;
/// std::vector<Hit> hits = //...;
//a
/// SimpleXFit fit(track, hits);
/// // check for error
/// if (!fit) throw;
/// // okay, fit converged, get reduced chi^2
/// double chi2ndf = fit.chi2(track, hits) / fit.ndf();
/// // exclude a hit and refit
/// hits[42].valid = false;
/// fit.refit(track, hits);
/// // check for error
/// if (!fit) throw;
/// // all went well, do something with the track
/// @endcode
/// That's it. All done. ;)
///
/// @section impl_notes Implementation Notes
/// @subsection on_autovect On Auto-vectorisation
/// Clearly, changing all the track and hit classes to have a memory layout that
/// is compatible with the requirements of present-day CPUs is an enormous task;
/// the technical difficulties to overcome on this path are considerable.
/// Rather than solving this problem, SIMDChi2Fit tries to address the simpler
/// issue of exploiting the parallelism in updating inverse covariance matrix
/// and the right hand side of a linear system by preforming identical
/// calculations on a number of hits in parallel. The preferred memory layout
/// for such an endeavour is well known: Essentially, one has to form, for each
/// entry of the inverse covariance matrix and the right hand side, a
/// dot-product of vectors. These vectors have one component for each hit. The
/// task at hand thus boils
/// down to:
/// - for each valid hit, compute the gradient and residual
/// - rearrange these quantities into SIMD-friendly vectors (see above)
/// - based on those SIMD-friendly vectors, update right hand side and inverse
///   covariance matrix (see SIMDChi2Fit::accumulate)
/// - then solve the linear system as usual
///
/// @subsection defer_cov On deferred covariance matrix calculation
/// To get out the covariance matrix without too much overhead, and to only
/// spend time/effort/resources on it when it's actually requested by the
/// caller, one has to jump through quite a few hoops. A few structs and unions
/// implement deferred inversion to obtain the covariance matrix; this is a
/// tricky thing to design, so I give a couple of notes on my reasoning
/// below...
///
/// Goals:
/// - it should be possible to disable this feature for fits that don't
///   need the covariance matrix through CommonFitPolicy; in that case, no
///   storage or runtime overhead should be incurred
/// - when this feature is enabled, it should
///   - save capture the state of the decomposition in the final
///     iteration of the fit (when the TrackUpdater says things have
///     converged)
///   - actually inverting the covariance matrix is a lot of work, this
///     should be deferred until it is really needed
///   - there should not be any dynamic memory management to keep things
///     fast
///
/// Implementation notes:
/// - std::function is out because it has to call new or something similar
/// - even if I could figure out the right type for the lambda that
///   captures the decomposition and can be used to perform the deferred
///   matrix inversion, there is a problem that the variable to hold
///   that lambda needs to be initialised at class construction; we want
///   to avoid that overhead (and just set a bool indicating if things
///   are initialised)
/// - this leads to the following decisions:
///   - capturing the state of the decomposition and supporting the
///     deferred inversion is done with a functor, DeferredCovariance
///   - we need to work around the need to initialise the instance of
///     DeferredCovariance until we have something to fill it with; this
///     is achieved by putting it into a union, DeferredCovarianceUnion
///   - a flag keeps track of wheather the DeferredCovariance instance
///     is initialised
///   - when the deferredCovariance feature is disabled, we use an empty
///     struct, Empty, to replace the DeferredCovariance instance and the
///     flag; its methods are no-ops that allow the code inside
///     SIMDChi2Fit's methods to remain unchanged
///   - the publically visible cov() method is enabled/disabled with
///     std::enable_if
///
/// @todo SIMDChi2Fit as it is now is designed for one-dimensional measurement
/// spaces (hits are essentially a line); for pixel (2D) or spacepoints (3D),
/// some redesign will be needed...
template<
    class MEASUREMENTPROVIDER, class MEASUREMENTPROJECTOR,
    class TRACKUPDATER, class HITUPDATER, 
    unsigned NHITSMAX, unsigned SIMDWIDTH = 4 >
class SIMDChi2Fit : public MEASUREMENTPROVIDER::commonfitpolicy_type
{
    private:
	/// @brief class to wrap an iterator pair for use as a range
	template<class IT> struct iter_pair_range : public std::pair<IT, IT> {
	    iter_pair_range(IT&& begin, IT&& end) :
		std::pair<IT, IT>(std::forward(begin), std::begin(end))
	    { }
	    IT begin() const { return std::pair<IT, IT>::first; }
	    IT end() const { return std::pair<IT, IT>::second; }
	};
	/// @brief construct a range from an iterator pair
	template <class IT>
	    static iter_pair_range<IT> as_range(IT&& begin, IT&& end)
	    { return iter_pair_range<IT>(
		    std::forward(begin), std::forward(end)); }

    public:
	COMMONPOLICY_INJECT_TYPES(MEASUREMENTPROVIDER::commonfitpolicy_type);
	static_assert(
		std::is_same<typename MEASUREMENTPROJECTOR::commonfitpolicy_type,
		commonfitpolicy_type>::value &&
		std::is_same<typename TRACKUPDATER::commonfitpolicy_type,
		commonfitpolicy_type>::value &&
		std::is_same<typename HITUPDATER::commonfitpolicy_type,
		commonfitpolicy_type>::value,
		"CommonFitPolicy types do not match.");

	/// @brief max. number of hits to add in one SIMD loop
	enum { nHitsMax = NHITSMAX };
	/// @brief max. width of SIMD operation (for padding)
	enum { simdWidth = SIMDWIDTH };
	static_assert(nHitsMax % simdWidth == 0,
		"nHitsMax must be multiple of simdWidth");

	/// @brief common fit policy
	typedef commonfitpolicy_type CommonFitPolicy;
	/// @brief type which provides the measurement for a given hit
	typedef MEASUREMENTPROVIDER MeasurementProvider;
	/// @brief type which projects a track into the measurement space of a hit
	typedef MEASUREMENTPROJECTOR MeasurementProjector;
	/// @brief type which knows how to update track parameters
	typedef TRACKUPDATER TrackUpdater;
	/// @brief type which knows how to update hits after a track parameter change
	typedef HITUPDATER HitUpdater;

    private:
	matrix_type m_icov;	///< @brief (inverse) covariance matrix
	vector_type m_rhs;	///< @brief right hand side of problem
	int m_ndf;		///< @brief number of degrees of freedom
	FitStatus m_status;	///< @brief fit status


	/** @brief empty struct as a zero size placeholder for the deferred
	 * covariance matrix calculation machinery
	 */
	struct Empty {
	    Empty() { }
	    template <class T> Empty(const T&) { }
	    template <class T> void operator=(const T&) { }
	};

	/** @brief structure to capture the decomposition, so the inversion to
	 * obtain the covariance matrix can be deferred
	 */
	struct DeferredCovariance {
	    typedef ROOT::Math::CholeskyDecomp<value_type, nDim> decomp_type;
	    decomp_type m_decomp;
#if 0 // add back in once C++11 support has caught up
	    static_assert(std::is_trivially_destructible<decomp_type>::value,
		    "decomp_type's destructor has side-effects, "
		    "this is dangerous!");
#endif
	    DeferredCovariance(const decomp_type& decomp) : m_decomp(decomp)
	    { }
	    template<class T>
	    bool operator()(T& cov) const { return m_decomp.Invert(cov); }
	};

	/** @brief union to avoid having to initialise the deferred covariance
	 * calculation
	 */
	union DeferredCovarianceUnion {
	    DeferredCovarianceUnion() { }
	    typename std::conditional<deferredCovariance,
		DeferredCovariance, Empty>::type m_deferredCovariance;
	} m_deferredCovarianceUnion;
	/** @brief true if m_deferredCovarianceUnion contains a valid deferred
	 * calculation
	 */
	typename std::conditional<deferredCovariance, bool, Empty>::type
	    m_deferredCovarianceInitialised;

    public:
	/** @brief fit a track to the hits given in rangeOrContainer
	 *
	 * @param track		track whose parameters are to be updated
	 * @param rangeOrContainer
	 * 			hits to be fitted
	 */
	template<class RC>
	SIMDChi2Fit(track_type& track, RC& rangeOrContainer) noexcept :
	    m_status(doFit(track, rangeOrContainer))
	{ }

	/** @brief fit a track to the hits given in rangeOrContainer
	 *
	 * @param track		track whose parameters are to be updated
	 * @param begin		begin of range of hits to be fitted
	 * @param end		end of range of hits to be fitted
	 */
	template<class IT>
	SIMDChi2Fit(track_type& track, IT&& begin, IT&& end) :
	    SIMDChi2Fit(track, as_range(std::forward(begin), std::forward(end)))
	{ }

	/** @brief return true if the last fit converged
	 *
	 * @returns true if the fit converged and the hit has at least as many
	 * data points as degrees of freedom
	 */
	operator bool() const noexcept
	{
	    return (1 == nIterMax) ? (UpdateAccepted <= m_status) :
		(FitConverged <= m_status);
	}

	/** @brief return true if the last fit converged
	 *
	 * @returns true if the fit converged and the hit has at least as many
	 * data points as degrees of freedom
	 */
	bool ok() const noexcept { return *this; }

	/** @brief return status of last fit
	 *
	 * @returns status of last fit
	 */
	FitStatus status() const noexcept { return m_status; }

	/** @brief return number of degrees of freedom
	 *
	 * @returns number of degrees of freedom
	 *
	 * @note This is calculated as number of data poins minus number of fit
	 * parameters, so it can be negative if you try to fit too many
	 * parameters.
	 */
	int ndf() const noexcept { return m_ndf; }

	/** @brief calculate @f$\chi^2@f$ contribution of a hit
	 *
	 * @param track		track with respect to which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 * @param hit		hit for which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 *
	 * @returns @f$\chi^2@f$ contribution
	 *
	 * @note please check that the last fit went ok()
	 *
	 * @note Hits for which MeasurementProvider().valid(hit) is false are
	 * skipped.
	 */
	value_type chi2(const track_type& track, const hit_type& hit) const noexcept
#if defined(__GNUC__)
	    // force gcc and friends to optimise aggressively
	    __attribute__((optimize("-ffast-math", "-ftree-vectorize", "-fvect-cost-model", "-ftree-slp-vectorize", "-ftree-vect-loop-version"), flatten));
#else
	    ;
#endif

	/** @brief calculate @f$\chi^2@f$ contribution of a range of hits
	 *
	 * @param track		track with respect to which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 * @param begin		start of range of hits for which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 * @param end		end of range of hits for which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 *
	 * @returns @f$\chi^2@f$ contribution
	 *
	 * @note please check that the last fit went ok()
	 *
	 * @note Hits for which MeasurementProvider().valid(hit) is false are
	 * skipped.
	 */
	template <class IT>
	value_type chi2(const track_type& track, const IT& begin, const IT& end) const noexcept
	{
	    static_assert(std::is_convertible<
		    decltype(*begin), const hit_type&>::value,
		    "Iterator type IT does not deference to correct type");
	    return std::accumulate(begin, end, value_type(0),
		    [&](value_type chi2sum, const hit_type& hit)
		    { return chi2sum + this->chi2(track, hit); });
	}

	/** @brief calculate @f$\chi^2@f$ contribution of a range of a
	 * container or range
	 *
	 * @param track		track with respect to which to calculate
	 * 			the @f$\chi^2@f$ contribution
	 * @param containerOrRange
	 * 			hits for which to calculate the @f$\chi^2@f$
	 *			contribution
	 *
	 * @returns @f$\chi^2@f$ contribution
	 *
	 * @note please check that the last fit went ok()
	 *
	 * @note Hits for which MeasurementProvider().valid(hit) is false are
	 * skipped.
	 */
	template <class C>
	value_type chi2(const track_type& track, const C& containerOrRange) const noexcept
	{ return chi2(track,
		std::begin(containerOrRange), std::end(containerOrRange)); }

	/** @brief refit a track to the hits given in rangeOrContainer
	 *
	 * @param track		track whose parameters are to be updated
	 * @param rangeOrContainer
	 * 			hits to be fitted
	 *
	 * @returns fit status
	 *
	 * Use this routine to repeat the computations done in the constructor.
	 */
	template<class RC>
	FitStatus refit(track_type& track, RC& rangeOrContainer) noexcept
	{ return (m_status = doFit(track, rangeOrContainer)); }

	/** @brief refit a track to the hits given in rangeOrContainer
	 *
	 * @param track		track whose parameters are to be updated
	 * @param begin		begin of range of hits to be fitted
	 * @param end		end of range of hits to be fitted
	 *
	 * @returns fit status
	 *
	 * Use this routine to repeat the computations done in the constructor.
	 */
	template<class IT>
	FitStatus refit(track_type& track, IT&& begin, IT&& end) noexcept
	{ return refit(track, as_range(std::forward(begin), std::forward(end))); }

	/** @brief add a single hit to inverse covariance matrix and right hand side
	 *
	 * @param track	track for which the fit is to be performed
	 * @param hit	hit to be added
	 *
	 * @note This routine does not refit the track, nor does it update hits
	 * after the track parameter update.
	 *
	 * The typical use case is for a perfectly linear track model where
	 * including/excluding single hits can be done without a full refit.
	 * Example:
	 * @code
	 * SIMDChi2Fit<...> fit(track, hits);
	 * // stop if the fit failed, or if we cannot remove a single hit
	 * if (!fit || fit.ndf() < 1) throw;
	 * // find the hit with the largest influence on chi2
	 * const Hit* worst = 0;
	 * const double chi2 = fit.chi2(track, hits);
	 * double chi2inc = -std::numeric_limits<double>::max();
	 * for (Hit& hit : hits) {
	 *     fit.removeHit(track, hit);
	 *     hit.setValid(false);
	 *     fit.refit(track);
	 *     double lchi2inc = fit.chi2(track, hits) - chi2;
	 *     if (lchi2inc > chi2inc) chi2inc = lchi2inc, worst = &hit;
	 *     hit.setValid(true);
	 *     fit.addHit(track, hit);
	 * }
	 * @endcode
	 */
	void addHit(track_type& track, const hit_type& hit) noexcept
	{ ++m_ndf; doAddRemoveHit(track, hit, +1); }

	/** @brief add a single hit to inverse covariance matrix and right hand side
	 *
	 * @param track	track for which the fit is to be performed
	 * @param hit	hit to be added
	 *
	 * @note This routine does not refit the track, nor does it update hits
	 * after the track parameter update.
	 *
	 * The typical use case is for a perfectly linear track model where
	 * including/excluding single hits can be done without a full refit.
	 * Example:
	 * @code
	 * SIMDChi2Fit<...> fit(track, hits);
	 * // stop if the fit failed, or if we cannot remove a single hit
	 * if (!fit || fit.ndf() < 1) throw;
	 * // find the hit with the largest influence on chi2
	 * const Hit* worst = 0;
	 * const double chi2 = fit.chi2(track, hits);
	 * double chi2inc = -std::numeric_limits<double>::max();
	 * for (Hit& hit : hits) {
	 *     fit.removeHit(track, hit);
	 *     hit.setValid(false);
	 *     fit.refit(track);
	 *     double lchi2inc = fit.chi2(track, hits) - chi2;
	 *     if (lchi2inc > chi2inc) chi2inc = lchi2inc, worst = &hit;
	 *     hit.setValid(true);
	 *     fit.addHit(track, hit);
	 * }
	 * @endcode
	 */
	void removeHit(track_type& track, const hit_type& hit) noexcept
	{ --m_ndf; doAddRemoveHit(track, hit, -1); }
	
	/** @brief refit after adding/removing single hits
	 *
	 * @param track	track for which the refit is to be performed
	 *
	 * @returns fit status
	 *
	 * @note This routine refits the track, but does not update hits after
	 * the track parameter update.
	 *
	 * The typical use case is for a perfectly linear track model where
	 * including/excluding single hits can be done without a full refit.
	 * Example:
	 * @code
	 * SIMDChi2Fit<...> fit(track, hits);
	 * // stop if the fit failed, or if we cannot remove a single hit
	 * if (!fit || fit.ndf() < 1) throw;
	 * // find the hit with the largest influence on chi2
	 * const Hit* worst = 0;
	 * const double chi2 = fit.chi2(track, hits);
	 * double chi2inc = -std::numeric_limits<double>::max();
	 * for (Hit& hit : hits) {
	 *     fit.removeHit(track, hit);
	 *     hit.setValid(false);
	 *     fit.refit(track);
	 *     double lchi2inc = fit.chi2(track, hits) - chi2;
	 *     if (lchi2inc > chi2inc) chi2inc = lchi2inc, worst = &hit;
	 *     hit.setValid(true);
	 *     fit.addHit(track, hit);
	 * }
	 * @endcode
	 */
	FitStatus refit(track_type& track) noexcept
	{ return (m_status = solveAndUpdate(track)); }

	/** @brief return the covariance matrix of the last fit
	 *
	 * @param cov	where to store the covariance matrix
	 *
	 * @returns true on success, false on failure
	 *
	 * @note if deferredCovariance is false (set via commonfitpolicy_type),
	 * this method is absent.
	 */
	template<class T>
	bool cov(typename std::enable_if<
		deferredCovariance, T>::type& cov) const noexcept
	{
	    // check if we have a deferred covariance calculation stored
	    if (m_deferredCovarianceInitialised && ok()) {
		// yes, do the calculation
		return m_deferredCovarianceUnion.m_deferredCovariance(cov);
	    } else {
		// no, refuse to work
		return false;
	    }
	}

    private:
	/// fill m_icov and m_rhs with initial values
	void reset(const track_type& track) noexcept
	{
	    m_icov = MeasurementProjector().seedICov(track);
	    m_rhs = MeasurementProjector().seedRhs(track);
	    m_ndf = -nDim;
	    m_status = NotPosDef;
	    m_deferredCovarianceInitialised = false;
	}

	/// helper to implement addHit and removeHit
	void doAddRemoveHit(track_type& track, const hit_type& hit,
		value_type w) noexcept
	{
	    const value_type wproj = MeasurementProjector().wproj(track, hit);
	    const value_type wgrad = MeasurementProjector().wgrad(track, hit);
	    const value_type wmeas =
		MeasurementProvider().wmeas(hit, track, MeasurementProjector());
	    for (unsigned i = 0, idx = 0; LIKELY(i != nDim); ++i) {
		// update m_rhs
		m_rhs[i] += w * wgrad[i] * (wmeas - wproj);
		// update m_icov
		for (unsigned j = 0; LIKELY(j <= i); ++j, ++idx) {
		    m_icov[idx] += w * wgrad[i] * wgrad[j];
		}
	    }
	}

	/// helper to add hits to fit - autovectorisable
	void accumulate(unsigned iHitMax,
		const std::array<std::array<value_type, nHitsMax>, nDim>& wgrads,
		const std::array<value_type, nHitsMax>& wmeass) noexcept
#if defined(__GNUC__)
	    // force gcc and friends to optimise aggressively
	    __attribute__((optimize("-ffast-math", "-ftree-vectorize",
			    "-fvect-cost-model", "-ftree-slp-vectorize",
			    "-ftree-vect-loop-version", "-funroll-all-loops"),
		       	flatten));
#else
	    ;
#endif

	/// helper to add hits to track
	template<class C>
	void addHits(const track_type& track, const C& hits) noexcept
#if defined(__GNUC__)
	    // force gcc and friends to optimise aggressively
	    __attribute__((optimize("-ffast-math", "-ftree-vectorize",
			    "-fvect-cost-model", "-ftree-slp-vectorize",
			    "-ftree-vect-loop-version", "-funroll-all-loops"),
		       	flatten));
#else
	    ;
#endif

	/// solve and update the track
	FitStatus solveAndUpdate(track_type& track) noexcept
#if defined(__GNUC__)
	    // force gcc and friends to optimise aggressively
	    __attribute__((optimize("-ffast-math", "-ftree-vectorize",
			    "-fvect-cost-model", "-ftree-slp-vectorize",
			    "-ftree-vect-loop-version"), flatten));
#else
	    ;
#endif

	/// solve and update the track and hits
	template<class C>
	FitStatus solveAndUpdate(track_type& track, C& hits) noexcept
	{
	    FitStatus retVal = solveAndUpdate(track);
	    if (LIKELY(UpdateAccepted <= retVal)) {
		for (hit_type& hit : hits) HitUpdater().updateHit(hit, track);
	    }
	    return retVal;
	}

	/// iterate track fit once
	template<class C>
	FitStatus doFitOneIter(track_type& track, C& hits) noexcept
	{
	    reset(track);
	    addHits(track, hits);
	    return solveAndUpdate(track, hits);
	}
	/// iterate track fit the specified number of times
	template<class C>
	FitStatus doFit(track_type& track, C& hits) noexcept
	{
	    FitStatus retVal = UpdateRejected;
	    for (unsigned iter = 0; LIKELY(iter != nIterMax); ++iter) {
		retVal = doFitOneIter(track, hits);
		if (UNLIKELY(UpdateAccepted != retVal)) break;
	    }
	    return retVal;
	}
};

template<
    class MEASUREMENTPROVIDER, class MEASUREMENTPROJECTOR,
    class TRACKUPDATER, class HITUPDATER, 
    unsigned NHITSMAX, unsigned SIMDWIDTH>
typename SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
     TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::value_type
SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
     TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::chi2(
	     const track_type& track, const hit_type& hit) const noexcept
{
    if (UNLIKELY(!MeasurementProvider().valid(hit))) return value_type(0);
    const value_type wproj = MeasurementProjector().wproj(track, hit);
    const value_type wmeas =
	MeasurementProvider().wmeas(hit, track, MeasurementProjector());
    return (wmeas - wproj) * (wmeas - wproj);
}

template<
    class MEASUREMENTPROVIDER, class MEASUREMENTPROJECTOR,
    class TRACKUPDATER, class HITUPDATER, 
    unsigned NHITSMAX, unsigned SIMDWIDTH>
void SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
     TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::accumulate(unsigned iHitMax,
	     const std::array<std::array<value_type, nHitsMax>, nDim>& wgrads,
	     const std::array<value_type, nHitsMax>& wmeass) noexcept
{
    for (unsigned i = 0, idx = 0; LIKELY(i != nDim); ++i) {
	// update m_rhs
	// vectorises
#if !defined(__GNUC__)
	m_rhs[i] += std::inner_product(
		std::begin(wgrads[i]), std::begin(wgrads[i]) + iHitMax,
		std::begin(wmeass), value_type(0));
#else
	m_rhs[i] += vect::dot_product<value_type, 2>(
		std::begin(wgrads[i]), std::begin(wgrads[i]) + iHitMax,
		std::begin(wmeass), value_type(0));
#endif
	// update m_icov
	for (unsigned j = 0; LIKELY(j <= i); ++j, ++idx) {
	    // vectorises
#if !defined(__GNUC__)
	    m_icov[idx] += std::inner_product(
	    	    std::begin(wgrads[i]), std::begin(wgrads[i]) + iHitMax,
	    	    std::begin(wgrads[j]), value_type(0));
#else
	    m_icov[idx] += vect::dot_product<value_type, 2>(
		    std::begin(wgrads[i]), std::begin(wgrads[i]) + iHitMax,
		    std::begin(wgrads[j]), value_type(0));
#endif
	}
    }
}

template<
    class MEASUREMENTPROVIDER, class MEASUREMENTPROJECTOR,
    class TRACKUPDATER, class HITUPDATER, 
    unsigned NHITSMAX, unsigned SIMDWIDTH>
template<class C>
void SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
     TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::addHits(
	     const track_type& track, const C& hits) noexcept
{
    using namespace SIMDChi2FitImplDetail;
    typedef std::array<value_type, nHitsMax>
#if defined(__GNUC__)
	// align workspace arrays to cache lines to produce better SIMD code
       	__attribute__((aligned(64)))
#endif
       	AlignedArray;
    std::array<AlignedArray, nDim> wgrads;
    const auto wgradsT = make_transposed_view<nDim>(wgrads);
    AlignedArray wmeass;
    unsigned iHit = 0;
    // loop over all hits
    for (const hit_type& hit : hits) {
	// skip deselected hits
	if (UNLIKELY(!MeasurementProvider().valid(hit))) continue;
	++m_ndf;
	wgradsT[iHit] = MeasurementProjector().wgrad(track, hit);
	const value_type wproj =
	    MeasurementProjector().wproj(track, hit);
	const value_type wmeas = MeasurementProvider().wmeas(
		hit, track, MeasurementProjector());
	wmeass[iHit] = wmeas - wproj;
	if (UNLIKELY(++iHit == nHitsMax)) {
	    // ok, array fully filled, accumulate
	    accumulate(nHitsMax, wgrads, wmeass);
	    iHit = 0;
	}
    }
    if (LIKELY(iHit)) {
	// pad wgrads and wmeass to next multiple of simdWidth...
	const unsigned iHitMax = (0 == iHit % simdWidth) ? iHit :
	    (simdWidth * (iHit / simdWidth + 1));
	assert(iHitMax <= nHitsMax);
	std::fill(std::begin(wmeass) + iHit,
		std::begin(wmeass) + iHitMax, value_type(0));
	for (unsigned j = 0; LIKELY(j != nDim); ++j)
	    std::fill(std::begin(wgrads[j]) + iHit, 
		    std::begin(wgrads[j]) + iHitMax, value_type(0));
	// ... and accumulate the rest
	accumulate(iHitMax, wgrads, wmeass);
    }
}

template<
    class MEASUREMENTPROVIDER, class MEASUREMENTPROJECTOR,
    class TRACKUPDATER, class HITUPDATER, 
    unsigned NHITSMAX, unsigned SIMDWIDTH>
typename SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
    TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::FitStatus
SIMDChi2Fit<MEASUREMENTPROVIDER, MEASUREMENTPROJECTOR,
    TRACKUPDATER, HITUPDATER, NHITSMAX, SIMDWIDTH>::solveAndUpdate(
	     track_type& track) noexcept
{
    if (UNLIKELY(m_ndf < 0)) return NotPosDef;
    ROOT::Math::CholeskyDecomp<value_type, nDim> decomp(m_icov.data());
    if (UNLIKELY(!decomp)) return NotPosDef;
    vector_type dparam(m_rhs);
    decomp.Solve(dparam);
    FitStatus retVal = TrackUpdater().updateTrack(track, dparam);
    if (deferredCovariance && UNLIKELY(retVal == FitConverged)) {
	m_deferredCovarianceUnion.m_deferredCovariance =
	    DeferredCovariance(decomp);
	m_deferredCovarianceInitialised = true;
    }
    return retVal;
}

} // namespace SIMDChi2FitUtils

} // namespace Tf

#endif // SIMDCHI2FIT_H

// vim: tw=78:sw=4:ft=cpp
