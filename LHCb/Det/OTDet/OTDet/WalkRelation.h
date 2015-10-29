#ifndef OTDET_WALKRELATION
#define OTDET_WALKRELATION

#include <cmath> // std::copysign
#include <algorithm> // std::min, std::max

#include "GaudiKernel/Kernel.h" // for LIKELY/UNLIKELY
#include "VectorClass/vectorclass.h"

#include "LHCbMath/ChebyshevApprox.h"

namespace OTDet
{
  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class WalkRelation
  {
  private:
    float m_upperend; ///< upper end of Chebyshev approximation interval
    float m_foff; ///< float version of m_off below
    float m_fdptfactor; ///< float version of m_dptfactor below
    ChebyshevApprox<float, 5> m_approx; ///< Chebyshev approximation of walk relation
    float m_famp; ///< float version of m_amp below
    float m_fovertau; ///< float version of m_overtau below
    double m_off;
    double m_amp;
    double m_tau;
    double m_dpt;
    double m_dptfactor;
    double m_overtau;

    inline static double vdtTanh(const double x) noexcept;
    inline static double fastTanh(const double x) noexcept;
    //inline int float2int(const double d ) const;
    //inline int floattoint(const double d ) const;

  public:
    WalkRelation();
    WalkRelation(double off, double amp, double tau, double dpt);

    inline double walk(double l) const noexcept;
    inline float walk(float l) const noexcept;
    
    /** @brief get value of walk correction (fast routine)
     *
     * Internally, a truncated Chebyshev expansion is used, and its accuracy is
     * usually better than 0.080 ns for the default walk correction
     * parametrisation, and is considered totally safe for deviations from
     * ideal up to about half an OTIS bin.
     *
     * @note Half an OTIS bin is tiny compared to the time resolution from
     * particle drift, around 3 ns, and the latter determines the position
     * resolution. The error introduced by this approximation is thus
     * negligible while less than 5% of the resolution (and is usually much
     * less, i.e. 2%).
     */
    inline float walk_fast(float l) const noexcept;

    inline float walk_fast_error_estimate() const noexcept
    { return m_approx.error_estimate(); }

    inline double getOff() const noexcept { return m_off; }
    inline double getAmp() const noexcept { return m_amp; }
    inline double getTau() const noexcept { return m_tau; }
    inline double getDpt() const noexcept { return m_dpt; }

    // make these two public because other code might want to "borrow" them
    static float padeTanh(const float x) noexcept;
    static double padeTanh(const double x) noexcept;
  };

  inline double WalkRelation::padeTanh(const double x) noexcept
  {
    // for very large |x| > 20, tanh(x) is x/|x| anyway (at least to double
    // precision)
    //
    // NB: branch-free code takes longer to execute
    if (UNLIKELY(std::abs(x) > 20.)) return std::copysign(1., x);
    // strategy for large arguments: tanh(2x) = 2 tanh(x)/(1 + tanh^2(x))
    // idea is to use this "argument halving" a couple of times, and use a
    // very short Padé approximation for the rest of the way
    const auto xx = x * 0.125;
    const auto xx2 = xx * xx;
#if (0 || defined(__ROOTCLING__)) // pedestrian code
    const auto numer = 135135 + xx2 * (17325 + xx2 * ( 378 + xx2 *  1));
    const auto denom = 135135 + xx2 * (62370 + xx2 * (3150 + xx2 * 28));

    auto tanh = xx * numer / denom;
#else // same code SIMD'ified
    auto sum = xx2 * Vec2d{ 1, 28 } + Vec2d{ 378, 3150 };
    sum = xx2 * sum + Vec2d{ 17325, 62370 };
    sum = xx2 * sum + Vec2d{ 135135, 135135 };
    auto tanh = xx * sum[0] / sum[1];
#endif
    tanh = 2 * tanh / (tanh * tanh + 1);
    tanh = 2 * tanh / (tanh * tanh + 1);
    return 2 * tanh / (tanh * tanh + 1);
  }

  inline float WalkRelation::padeTanh(const float x) noexcept
  {
    // same strategy as double version above, but even shorter Padé
    // approximation is sufficient for float
    //
    // NB: branch-free code takes longer to execute
    if (UNLIKELY(std::abs(x) > 9.1f)) return std::copysign(1.f, x);
    const auto xx = x * 0.125f;
    const auto xx2 = xx * xx;
#if (1 || defined(__ROOTCLING__)) // pedestrian code
    auto tanh = xx * (xx2 + 15) / (6 * xx2 + 15);
#else // same code SIMD'ified - same speed as pedestrian code above
    // unfortunately, there's only a variant that works on four floats at a
    // time, so we (have to) leave half the SIMD register empty
    typedef float fv4 __attribute__ ((vector_size(16)));
    const fv4 sum = xx2 * fv4{ 1, 6 } + fv4{ 15, 15 };
    auto tanh = xx * sum[0] / sum[1];
#endif
    tanh = 2 * tanh / (tanh * tanh + 1);
    tanh = 2 * tanh / (tanh * tanh + 1);
    return 2 * tanh / (tanh * tanh + 1);
  }

  inline double WalkRelation::walk(double l) const noexcept
  {
    l = std::max(l, 0.); // only positive values allowed
    const double tanh = padeTanh(l * m_overtau);
    return (m_amp * (tanh - 1.0)) + (l * m_dptfactor + m_off);
  }

  inline float WalkRelation::walk(float l) const noexcept
  {
    l = std::max(l, 0.f); // only positive values allowed
    const float tanh = padeTanh(l * m_fovertau);
    return (m_famp * tanh - m_famp) + (l * m_fdptfactor + m_foff);
  }

  inline float WalkRelation::walk_fast(float l) const noexcept
  {
    // if l > 9.1 * m_tau, then tanh(x) is 1 to float precision anyway, so we can
    // get by with a linear approximation; typical values for l and m_tau will
    // not produce such a large result, and we make do with a Chebyshev
    // approximation of the full monty...
    return UNLIKELY(l > m_upperend) ? (m_fdptfactor * l + m_foff) :
      m_approx(std::max(0.f, l));
  }
}

#endif // OTDET_WALKRELATION
