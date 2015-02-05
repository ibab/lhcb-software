#ifndef OTDET_WALKRELATION
#define OTDET_WALKRELATION

#include "LHCbMath/ChebyshevApprox.h"

namespace OTDet
{
  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class WalkRelation
  {
  private:
    /// pointer to implementation of fast walk correction
    float (WalkRelation::*m_walk_fast)(const float) const noexcept;
    float m_upperend; ///< upper end of Chebyshev approximation interval
    ChebyshevApprox<float, 5> m_approx; ///< Chebyshev approximation of walk relation
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

    float walk_fast_chebyshev(float l) const noexcept;
    float walk_fast_exact(float l) const noexcept;
    float walk_fast_zero(float l) const noexcept;

  public:
    WalkRelation();
    WalkRelation(double off, double amp, double tau, double dpt);

    double walk(double l) const noexcept;
    
    /** @brief get value of walk correction (fast routine)
     *
     * Internally, a truncated Chebyshev expansion is used, and its accuracy is
     * usually better than 0.080 ns for the default walk correction
     * parametrisation. The implementation monitors the maximum error from the
     * exact solution, and if it is larger than half an OTIS bin, it falls back
     * on the full calculation.
     *
     * @note Half an OTIS bin is tiny compared to the time resolution from
     * particle drift, around 3 ns, and the latter determines the position
     * resolution. The error introduced by this approximation is thus
     * guaranteed to be less than 5% of the resolution (and is usuall much
     * less, i.e. 2%).
     */
    inline float walk_fast(float l) const noexcept
    { return (this->*m_walk_fast)(l); }

    inline double getOff() const noexcept { return m_off; }
    inline double getAmp() const noexcept { return m_amp; }
    inline double getTau() const noexcept { return m_tau; }
    inline double getDpt() const noexcept { return m_dpt; }

    // make these two public because other code might want to "borrow" them
    static float padeTanh(const float x) noexcept;
    static double padeTanh(const double x) noexcept;
  };
}

#endif // OTDET_WALKRELATION
