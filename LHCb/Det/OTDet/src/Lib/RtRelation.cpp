#include "OTDet/RtRelation.h"
#include <cmath>
#include <iostream>

/// little helper routines
namespace {
  /// little helper routine to get coefficients of derivative of given polynomial
  template <typename FLT, typename RETFLT = FLT>
  static std::vector<RETFLT> derivative(const std::vector<FLT>& coeffs, const FLT scale = 1)
  {
    if (2 > coeffs.size()) return std::vector<RETFLT>{RETFLT(0)};
    std::vector<RETFLT> retVal(std::begin(coeffs) + 1, std::end(coeffs));
    unsigned k = 0;
    for (RETFLT& c: retVal) c *= scale * ++k;
    return retVal;
  }
}

namespace OTDet
{

  RtRelation::RtRelation(float rmax,
			 const std::vector<float>& tcoeff,
			 const std::vector<float>& terrcoeff) :
    m_rmax(rmax), m_rconv(1 / rmax),
    m_tcoeff(std::begin(tcoeff), std::end(tcoeff)),
    m_dtdrcoeff(derivative<float, float>(tcoeff, 1 / rmax)),
    m_terrcoeff(std::begin(terrcoeff), std::end(terrcoeff)),
    m_tmin(drifttime(0.)), m_tmax(drifttime(m_rmax)),
    m_rt(ChebyshevApprox<float, s_ChebTerms>::inverse(0.f, rmax,
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_drdt(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0.f, rmax,
	  [this] (float rr) { return 1.f / dtdr(rr); },
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_sigmar(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0.f, rmax,
	  [this] (float rr) { return float(drifttimeError(rr) / std::abs(dtdr(rr))); },
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_drdtAtRmin(1 / std::abs(dtdr(0.f))),
    m_drdtAtRmax(1 / std::abs(dtdr(m_rmax)))
  {
    // "normalise" polynomials by chopping off zero coefficients at the back
    while (m_tcoeff.size() > 1 && 0.f == m_tcoeff.back()) m_tcoeff.pop_back();
    while (m_terrcoeff.size() > 1 && 0.f == m_terrcoeff.back()) m_terrcoeff.pop_back();
    while (m_dtdrcoeff.size() > 1 && 0.f == m_dtdrcoeff.back()) m_dtdrcoeff.pop_back();
  }

  RtRelation::RtRelation(float rmax,
      const std::vector<float>& tcoeff,
      float sigmaR) :
    m_rmax(rmax), m_rconv(1 / rmax),
    m_tcoeff(std::begin(tcoeff), std::end(tcoeff)),
    m_dtdrcoeff(derivative<float, float>(tcoeff, 1 / rmax)),
    // if we want constant error in r, we must put dt/dr = sigma_r * t'(r)
    m_terrcoeff(derivative<float, float>(tcoeff, sigmaR / rmax)),
    m_tmin(drifttime(0.)), m_tmax(drifttime(m_rmax)),
    m_rt(ChebyshevApprox<float, s_ChebTerms>::inverse(0.f, rmax,
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_drdt(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0.f, rmax,
	  [this] (float rr) { return 1.f / dtdr(rr); },
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_sigmar(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0.f, rmax,
	  [sigmaR] (float /* rr */) { return sigmaR; },
	  [this] (float rr) { return float(drifttime(rr)); })),
    m_drdtAtRmin(1 / std::abs(dtdr(0.f))),
    m_drdtAtRmax(1 / std::abs(dtdr(m_rmax)))
  {
    // "normalise" polynomials by chopping off zero coefficients at the back
    while (m_tcoeff.size() > 1 && 0.f == m_tcoeff.back()) m_tcoeff.pop_back();
    while (m_terrcoeff.size() > 1 && 0.f == m_terrcoeff.back()) m_terrcoeff.pop_back();
    while (m_dtdrcoeff.size() > 1 && 0.f == m_dtdrcoeff.back()) m_dtdrcoeff.pop_back();
  }

  RtRelation::RtRelation(double rmax,
			 const std::vector<double>& tcoeff,
			 const std::vector<double>& terrcoeff) :
    m_rmax(rmax), m_rconv(1 / rmax),
    m_tcoeff(std::begin(tcoeff), std::end(tcoeff)),
    m_dtdrcoeff(derivative<double, float>(tcoeff, 1 / rmax)),
    m_terrcoeff(std::begin(terrcoeff), std::end(terrcoeff)),
    m_tmin(drifttime(0.)), m_tmax(drifttime(m_rmax)),
    m_rt(ChebyshevApprox<float, s_ChebTerms>::inverse(0., rmax,
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_drdt(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0., rmax,
	  [this] (double rr) { return 1. / dtdr(rr); },
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_sigmar(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0., rmax,
	  [this] (double rr) { return double(drifttimeError(rr) / std::abs(dtdr(rr))); },
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_drdtAtRmin(1 / std::abs(dtdr(0.f))),
    m_drdtAtRmax(1 / std::abs(dtdr(m_rmax)))
  {
    // "normalise" polynomials by chopping off zero coefficients at the back
    while (m_tcoeff.size() > 1 && 0.f == m_tcoeff.back()) m_tcoeff.pop_back();
    while (m_terrcoeff.size() > 1 && 0.f == m_terrcoeff.back()) m_terrcoeff.pop_back();
    while (m_dtdrcoeff.size() > 1 && 0.f == m_dtdrcoeff.back()) m_dtdrcoeff.pop_back();
  }

  RtRelation::RtRelation(double rmax,
      const std::vector<double>& tcoeff,
      double sigmaR) :
    m_rmax(rmax), m_rconv(1 / rmax),
    m_tcoeff(std::begin(tcoeff), std::end(tcoeff)),
    m_dtdrcoeff(derivative<double, float>(tcoeff, 1 / rmax)),
    // if we want constant error in r, we must put dt/dr = sigma_r * t'(r)
    m_terrcoeff(derivative<double, float>(tcoeff, sigmaR / rmax)),
    m_tmin(drifttime(0.)), m_tmax(drifttime(m_rmax)),
    m_rt(ChebyshevApprox<float, s_ChebTerms>::inverse(0., rmax,
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_drdt(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0., rmax,
	  [this] (double rr) { return 1. / dtdr(rr); },
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_sigmar(ChebyshevApprox<float, s_ChebTerms - 1>::implicit(0., rmax,
	  [sigmaR] (double /* rr */) { return sigmaR; },
	  [this] (double rr) { return double(drifttime(rr)); })),
    m_drdtAtRmin(1 / std::abs(dtdr(0.f))),
    m_drdtAtRmax(1 / std::abs(dtdr(m_rmax)))
  {
    // "normalise" polynomials by chopping off zero coefficients at the back
    while (m_tcoeff.size() > 1 && 0.f == m_tcoeff.back()) m_tcoeff.pop_back();
    while (m_terrcoeff.size() > 1 && 0.f == m_terrcoeff.back()) m_terrcoeff.pop_back();
    while (m_dtdrcoeff.size() > 1 && 0.f == m_dtdrcoeff.back()) m_dtdrcoeff.pop_back();
  }

  RtRelation::RtRelation() : RtRelation(2.45, { 0., 20.1, 14.4 }, { 3.0, 0.67 } )
  { }

  //   RtRelation::RtRelation(double tmin, double tmax, const std::vector<double>& radius)
  //     : m_tmin(tmin), m_tmax(tmax), m_rtable(radius), m_dt( (tmax-tmin) / (radius.size()-1)), m_rmin(radius.front()), m_rmax(radius.back())
  //   {
  //     // now we still need to create the coefficients for the t(r)
  //     // function. the best is just to make a chebychev approximation
  //     // (evaluation on nodes) , but I don't have time to think about
  //     // that now. I'll just decapriacte this function for the time
  //     // being and make a linear approximation on the first and last
  //     // point. this makes only sense for the MC linear rt-relation (for
  //     // which we are never going to use it).
  //     double dtdr = (m_tmax-m_tmin) / (m_rmin - m_rmax) ;
  //     m_tcoeff = boost::assign::list_of(m_tmin - m_rmin * dtdr)(dtdr) ;
  //     std::cout << "This constructor is not complete. Please FIXME." << std::endl ;
  //   }

}
