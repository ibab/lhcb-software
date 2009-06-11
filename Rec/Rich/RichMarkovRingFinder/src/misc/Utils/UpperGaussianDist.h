
#ifndef LESTER_UPPERGAUSSIANDIST_H
#define LESTER_UPPERGAUSSIANDIST_H

// defines a gaussian which is truncated somewhere on the left hand side.
//The range of values which it can generate thus lies in [m_xMin,+infinity].

//fwd dec
#include "Utils/UpperGaussianDist.fwd"

//includes
#include "Utils/SampleAndProbabilityModule.h"
#include "Utils/MathsConstants.h"
#include "gsl/gsl_sf_erf.h"

namespace Lester
{

  //dec
  class UpperGaussianDist : public SampleAndProbabilityModule<double>
  {
  private:
    const double m_mean;
    const double m_sigma;
    /// m_xMin is the point of lower truncation
    const double m_xMin;
    const double m_oneOnRootTwoPiSigmaSq;
    double m_inverseTruncatedArea;
  public:
    static double inverseTruncatedArea(const double mean,
                                       const double sigma,
                                       const double xMin)
    {
      /// This method returns (one divided by) the area of a gaussian with mean and sigma as given, normalised to unit area, but then with the left hand part u7p to xMin lopped off!
      const double xMinInSigsFromMean  = (xMin-mean)/sigma;
      const double normalAreaAboveXMin = 0.5*gsl_sf_erfc(xMinInSigsFromMean/std::sqrt(2.0));
      return 1./normalAreaAboveXMin;
    };
  public:
    UpperGaussianDist(const double mean,
                      const double sigma,
                      const double xMin) : m_mean(mean),
                                           m_sigma(fabs(sigma)),
                                           m_xMin(xMin),
                                           m_oneOnRootTwoPiSigmaSq(1./(sqrt(MathsConstants::twoPi)*m_sigma))
    { 
      m_inverseTruncatedArea = this->inverseTruncatedArea(mean,m_sigma,xMin);
    }
    virtual double sample() const
    {
      double ans;
      do {
        ans = RandGauss::shoot(m_mean, m_sigma);
      } while (ans<=m_xMin);
      assert(ans>m_xMin);
      return ans;
    };
    virtual double probabilityOf(const double & x) const {
      if (x<=m_xMin) {
        return 0;
      } else {
        // first calcuate the prob we would get if the dist WASN'T truncated,
        // then divide by its truncated area.
        const double xInSigsFromMean = (x-m_mean)/m_sigma;
        const double xInSigsFromMeanSq = xInSigsFromMean*xInSigsFromMean;
        const double exponent = -0.5*xInSigsFromMeanSq;
        const double unTruncAns = m_oneOnRootTwoPiSigmaSq * exp(exponent);
        const double truncAns = unTruncAns * m_inverseTruncatedArea;
        return truncAns;
      };
    };
    double propToProbabilityOf(const double & sample) const {
      return probabilityOf(sample);
    };
    double logOfProbabilityOf(const double & x) const {
      if (x<=m_xMin) {
        throw Lester::LogarithmicTools::LogOfZero();
      } else {
        // first calcuate the prob we would get if the dist WASN'T truncated,
        // then divide by its truncated area.
        const double xInSigsFromMean = (x-m_mean)/m_sigma;
        const double xInSigsFromMeanSq = xInSigsFromMean*xInSigsFromMean;
        const double exponent = -0.5*xInSigsFromMeanSq;
        const double unTruncAnsLog = log(m_oneOnRootTwoPiSigmaSq) + exponent;
        const double truncAnsLog = unTruncAnsLog + log(m_inverseTruncatedArea);
        return truncAnsLog;
      };
      // return DEFAULT_logOfProbabilityOf(x);
    };
    double almostLogOfProbabilityOf(const double & sample) const
    {
      return logOfProbabilityOf(sample);
    };
    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "UpperGaussianDistribution[mean="<<m_mean<<", sigma="<<m_sigma<<", xmin="<<m_xMin << "]";
    };
    /*
      virtual double propToProbabilityOf(const double &) const;
      virtual double logOfProbabilityOf(const double &) const;
    */
  };

}

#endif
