
#include "Utils/UpperGaussianDist.h"

#include "CLHEP/Random/RandGauss.h"
#include <cmath>

/*
double UpperGaussianDist::sample() const {
  double ans;
  do {
    ans = RandGauss::shoot(m_mean, m_sigma);
  } while (ans<=m_xMin);
  assert(ans>m_xMin);
  return ans;
};
double UpperGaussianDist::probabilityOf(const double & x) const {
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
double UpperGaussianDist::propToProbabilityOf(const SampleType & double) const;
double UpperGaussianDist::logOfProbabilityOf(const SampleType & double) const;
*/
