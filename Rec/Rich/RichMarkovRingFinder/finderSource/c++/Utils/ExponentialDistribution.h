
#ifndef EXPONENTIALDISTRIBUTIONH
#define EXPONENTIALDISTRIBUTIONH

// fwd dec
#include "Utils/ExponentialDistribution.fwd" 

// includes
#include <iostream>
#include "Utils/SampleAndProbabilityModule.h"
#include "CLHEP/Random/RandExponential.h"
#include "Utils/MathsConstants.h"
#include "Utils/LogarithmicTools.h"

// declaration
struct ExponentialDistribution : public SampleAndProbabilityModule<double> {
private:
  const double m_mean;
  const double m_reciprocalMean;
  const double m_logReciprocalMean;
public:
  explicit ExponentialDistribution(const double mean=1) :
    m_mean(mean),
    m_reciprocalMean(1./mean),
    m_logReciprocalMean(-log(mean)) {
  };
  double sample() const {
    return RandExponential::shoot(m_mean);
  };
  double propToProbabilityOf(const double & d) const {
     if (d>=0) {
      return exp(-d*m_reciprocalMean);
    } else {
      return 0;
    };
  };
  double probabilityOf(const double & d) const {
     if (d>=0) {
      return m_reciprocalMean*exp(-d*m_reciprocalMean);
    } else {
      return 0;
    };
  };
  double almostLogOfProbabilityOf(const double & d) const {
    if (d>=0) {
      return -d*m_reciprocalMean;
    } else {
      throw Lester::LogarithmicTools::LogOfZero();
    };
  };
  double logOfProbabilityOf(const double & d) const {
    if (d>=0) {
      return m_logReciprocalMean-d*m_reciprocalMean;
    } else {
      throw Lester::LogarithmicTools::LogOfZero();
    };
  };
public:
  std::ostream & printMeTo(std::ostream & os) const {
    return os << "ExponentialDistribution[mean="<<m_mean<<"]";
  };
};

inline std::ostream & operator<<(std::ostream & os, const ExponentialDistribution & obj) {
  return obj.printMeTo(os);
};

#endif

