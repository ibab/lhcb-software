#ifndef OTDET_RTRELATION
#define OTDET_RTRELATION

#include <vector>
#include <algorithm>

#include "GaudiKernel/Kernel.h"

#include "LHCbMath/ChebyshevApprox.h"
#include "LHCbMath/PolyEval.h"

namespace OTDet
{
  /// little helper class to represent a value with its associated error
  struct ValueWithError
  {
    ValueWithError() : val(0), err(0) {}
    ValueWithError(float v, float e) : val(v), err(e) {}
    float val;
    float err;
  };

  /// convenience typedef
  typedef ValueWithError RadiusWithError;
  /// convenience typedef
  typedef ValueWithError DriftTimeWithError;
    
  /** @brief r(t) relation for the Outer Tracker.
   *
   * @author Wouter Hulsbergen
   * @date the mists of time
   *	- initial implementation with lookup tables to invert t(r) and friends
   *
   * @author Manuel Schiller
   * @date 2015-02-09
   * 	- move to Chebyshev polynomials and float precision instead of doubles;
   * 	  the precision is better with the new code
   */
  class RtRelation
  {
    public:
      /// default constructor. does nothing sensible.
      RtRelation();
      /// constructor from a polynomial t(r) and sigmat(r).  the
      /// coefficients are actually those of a polynomial in r/rmax,
      /// such that they all have unit [ns].
      RtRelation(float rmax, const std::vector<float>& tcoeff, const std::vector<float>& terrcoeff);
      RtRelation(double rmax, const std::vector<double>& tcoeff, const std::vector<double>& terrcoeff);
      ///  constructor from a polynomial t(r) and a fixed error in r. this is only for convenience with MC.
      RtRelation(float rmax, const std::vector<float>& tcoeff, float sigmaR);
      RtRelation(double rmax, const std::vector<double>& tcoeff, double sigmaR);

      // -------------------- various little helpers --------------------
      /// return maximum drift time
      inline float tmax() const noexcept { return m_tmax; }
      /// return minimum drift time
      inline float tmin() const noexcept { return m_tmin; }
      /// return minimum drift radius
      constexpr static inline float rmin() noexcept { return 0.f; }
      /// return maximum drift radius
      inline float rmax() const noexcept { return m_rmax; }

      /// return coefficients of polynimial t(r)
      inline const std::vector<float>& tcoeff() const noexcept
      { return m_tcoeff; }

      /// return coefficients of polynimial sigma_t(r)
      inline const std::vector<float>& terrcoeff() const noexcept
      { return m_terrcoeff; }

      typedef enum { Rt, Drdt, Rerr } WhichError;
      /// return error estimates of underlying Chebyshev approximations
      inline float error_estimate(WhichError which) const noexcept
      {
	return (Rt == which) ? m_rt.error_estimate() :
	  (Drdt == which) ? m_drdt.error_estimate() :
	  m_sigmar.error_estimate();
      }

      // -------------------- as function of radius --------------------
      /// return drifttime for given radius
      inline float drifttime(float radius) const noexcept;
      /// return derivative to radius
      inline float dtdr(float radius) const noexcept;
      /// return sigma(drifttime) for given radius
      inline float drifttimeError(float radius) const noexcept;

      /// return drifttime with error
      inline DriftTimeWithError drifttimeWithError(float radius) const noexcept;

      // -------------------- as function of drift time --------------------
      /// return radius for given drifttime
      inline float radius(float time) const noexcept;

      /// return radius for given drifttime. extrapolate into 'unphysical' region.
      inline float extrapolatedradius(float time) const noexcept;

      /// return radius for given drifttime
      inline float radiusError(float time) const noexcept;

      /// return derivative to time
      inline float drdt(float time) const noexcept;

      /// return average driftvelocity
      inline float drdt() const noexcept
      { return (rmax() - rmin()) / (tmax() - tmin()); }

      /// return radius for given drifttime
      inline RadiusWithError radiusWithError(float time) const noexcept;

    private:
      float m_rmax;		///< radius of inner edge straw wall [mm]
      float m_rconv;		///< cached 1.f / m_rmax [1/mm]
      std::vector<float> m_tcoeff;    ///< coefficients of polynomial t(r/rmax) all in [ns]
      std::vector<float> m_dtdrcoeff; ///< coefficients of dt/dr(r/rmax) all in [ns/1]
      std::vector<float> m_terrcoeff; ///< coefficients of polynomial sigma_t(r/rmax) all in [ns]

      float m_tmin;		///< cached t(r = 0) [ns]
      float m_tmax;		///< cached t(r = m_rmax) [ns]

      constexpr static unsigned s_ChebTerms = 5; ///< number of terms in Chebyshev approximations
      ChebyshevApprox<float, s_ChebTerms> m_rt; ///< approximation to r(t) relation
      ChebyshevApprox<float, s_ChebTerms - 1> m_drdt; ///< approximation to drdt(t) relation
      ChebyshevApprox<float, s_ChebTerms - 1> m_sigmar; ///< approximation to sigma_r(t) relation

      float m_drdtAtRmin; ///< dr/dt at r = 0
      float m_drdtAtRmax; ///< dr/dt at r = rmax
  };

  inline float RtRelation::drifttime(float radius) const noexcept
  {
    return PolyEval::eval(
	std::min(rmax(), std::max(rmin(), radius)) * m_rconv, m_tcoeff);
  }

  inline float RtRelation::dtdr(float radius) const noexcept
  {
    return PolyEval::eval(
	std::min(rmax(), std::max(rmin(), radius)) * m_rconv, m_dtdrcoeff);
  }

  inline float RtRelation::drifttimeError(float radius) const noexcept
  {
    return PolyEval::eval(
	std::min(rmax(), std::max(rmin(), radius)) * m_rconv, m_terrcoeff);
  }

  inline DriftTimeWithError RtRelation::drifttimeWithError(float radius) const noexcept
  {
    radius = std::min(rmax(), std::max(rmin(), radius)) * m_rconv;
    return DriftTimeWithError(PolyEval::eval(radius, m_tcoeff),
	PolyEval::eval(radius, m_terrcoeff));
  }

  inline float RtRelation::radius(float time) const noexcept
  {
    return m_rt(std::min(tmax(), std::max(tmin(), time)));
  }

  inline float RtRelation::extrapolatedradius(float time) const noexcept
  {
    return 
	UNLIKELY(time > tmax()) ? (rmax() + m_drdtAtRmax * (time - tmax())) :
	UNLIKELY(time < tmin()) ? (rmin() + m_drdtAtRmin * (time - tmin())) :
	m_rt(time);
  }

  inline float RtRelation::radiusError(float time) const noexcept
  {
    return m_sigmar(std::min(tmax(), std::max(tmin(), time)));
  }

  inline float RtRelation::drdt(float time) const noexcept
  {
    return m_drdt(std::min(tmax(), std::max(tmin(), time)));
  }

  inline RadiusWithError RtRelation::radiusWithError(float time) const noexcept
  {
    time = std::min(tmax(), std::max(tmin(), time));
    return RadiusWithError(m_rt(time), m_sigmar(time));
  }
}

#endif
