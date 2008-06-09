
#ifndef NORMALDISTRIBUTIONH
#define NORMALDISTRIBUTIONH

// fwd dec
#include "Utils/NormalDistribution.fwd"

// includes
#include <iostream>
#include "Utils/SampleAndProbabilityModule.h"
#include "CLHEP/Random/RandGauss.h"
#include "Utils/MathsConstants.h"

namespace Lester
{

  // declaration
  struct NormalDistribution : public SampleAndProbabilityModule<double> 
  {
  private:
    const double m_mean;
    const double m_sigma;
    const double m_sigmaSq;
    const double m_minusHalfOverSigmaSq;
    const double m_oneOverSqrtTwoPiSigSq;
    const double m_logOfOneOverSqrtTwoPiSigmaSq;
  public:
    explicit NormalDistribution(const double mean=0, const double sigma=1) :
      m_mean(mean),
      m_sigma(sigma),
      m_sigmaSq(sigma*sigma),
      m_minusHalfOverSigmaSq(-0.5/(m_sigmaSq)),
      m_oneOverSqrtTwoPiSigSq(1./sqrt(MathsConstants::twoPi*m_sigmaSq)),
      m_logOfOneOverSqrtTwoPiSigmaSq(log(m_oneOverSqrtTwoPiSigSq)) { }
    double sample() const 
    {
      return RandGauss::shoot(m_mean, m_sigma);
    }
    double propToProbabilityOf(const double & d) const 
    {
      double result(0);
      try 
      {
        result = std::exp( almostLogOfProbabilityOf(d) );
      } 
      catch (Lester::LogarithmicTools::LogOfZero) 
      {
        result = 0;
      }
      return result; 
    }
    double probabilityOf(const double & d) const 
    {
      return m_oneOverSqrtTwoPiSigSq*propToProbabilityOf(d);
    }
    double almostLogOfProbabilityOf(const double & d) const 
    {
      const double delta=d-m_mean;
      const double exponent = delta*delta*m_minusHalfOverSigmaSq;
      return exponent;
    }
    double logOfProbabilityOf(const double & d) const 
    {
      const double ans =
        almostLogOfProbabilityOf(d) + m_logOfOneOverSqrtTwoPiSigmaSq;
      return ans;
    }
  public:
    std::ostream & printMeTo(std::ostream & os) const {
      return os << "NormalDistribution[mean="<<m_mean<<", sigma="<<m_sigma<<"]";
    }
  };

  inline std::ostream & operator<<(std::ostream & os, 
                                   const NormalDistribution & obj) 
  {
    return obj.printMeTo(os);
  }

}

#endif

