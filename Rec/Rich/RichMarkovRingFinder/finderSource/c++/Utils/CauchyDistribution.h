
#ifndef CAUCHYDISTRIBUTIONH
#define CAUCHYDISTRIBUTIONH

// fwd dec
#include "CauchyDistribution.fwd"

// includes
#include <iostream>
#include "SampleAndProbabilityModule.h"
#include "CLHEP/Random/Randomize.h"
#include "MathsConstants.h"

namespace Lester
{

  // declaration
  struct CauchyDistribution : public SampleAndProbabilityModule<double> {
  private:
    const double m_mean;
    const double m_sigma;
    const double m_oneOverSigma;
    const double m_oneOverSigmaSq;
    const double m_oneOverSPi;
  public:
    explicit CauchyDistribution(const double mean=0, const double sigma=1) :
      m_mean(mean),
      m_sigma(sigma),
      m_oneOverSigma(1./sigma),
      m_oneOverSigmaSq(m_oneOverSigma*m_oneOverSigma),
      m_oneOverSPi(MathsConstants::oneOverPi*m_oneOverSigma) {
    };
    double sample() const {
      const double ans = m_mean + m_sigma*tan(RandFlat::shoot(-MathsConstants::piOverTwo,MathsConstants::piOverTwo));
      return ans;
    };
    double propToProbabilityOf(const double & d) const {
      const double delta=d-m_mean;
      const double one = 1;
      const double ans = one/(one + delta*delta*m_oneOverSigmaSq);
      return ans;
    };
    double probabilityOf(const double & d) const {
      return m_oneOverSPi*propToProbabilityOf(d);
    };
    double logOfProbabilityOf(const double & sample) const {
      return DEFAULT_logOfProbabilityOf(sample);
    };
    double almostLogOfProbabilityOf(const double & sample) const {
      return DEFAULT_almostLogOfProbabilityOf(sample);
    };
  public:
    std::ostream & printMeTo(std::ostream & os) const 
    {
      return os << "CauchyDistribution[mean="<<m_mean<<", sigma="<<m_sigma<<"]";
    }
    firend inline std::ostream & operator<<(std::ostream & os,
                                            const CauchyDistribution & obj)
    {
      return obj.printMeTo(os);
    }
  };

}

#endif

