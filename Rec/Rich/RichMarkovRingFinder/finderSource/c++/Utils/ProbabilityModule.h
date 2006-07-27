
#ifndef PROBABILITYMODULEH
#define PROBABILITYMODULEH

// fwd dec
#include "Utils/ProbabilityModule.fwd" 

// includes
#include "Utils/LogarithmicTools.h"
#include <iostream>

// declaration
template<class SampleType>
struct ProbabilityModule {
  virtual ~ProbabilityModule() {};

  virtual double probabilityOf(const SampleType &) const = 0;
  virtual double propToProbabilityOf(const SampleType & sample) const = 0;

  virtual double logOfProbabilityOf(const SampleType & sample) const = 0;
  virtual double almostLogOfProbabilityOf(const SampleType & sample) const = 0;
  virtual std::ostream & printMeTo(std::ostream & os) const = 0;

  //------ DEFAULTS BELOW FOR THE ABOVE.   USE IF NEEDED --------

  double DEFAULT_probabilityOf(const SampleType & sample) const {
    try {
      double logProb = logOfProbabilityOf(sample);
      return exp(logProb);
    } catch (Lester::LogarithmicTools::LogOfZero) {
      return 0;
    };
  };
  double DEFAULT_propToProbabilityOf(const SampleType & sample) const {
    try {
      double logProb = almostLogOfProbabilityOf(sample);
      return exp(logProb);
    } catch (Lester::LogarithmicTools::LogOfZero) {
      return 0;
    };
  };
  double DEFAULT_logOfProbabilityOf(const SampleType & sample) const {
    const double prob = probabilityOf(sample);
    if (prob==0) {
      throw Lester::LogarithmicTools::LogOfZero();
    };
    const double logProb = log(prob);
    if (!finite(logProb)) {
      throw Lester::LogarithmicTools::LogOfZero();      
    };
    return logProb;
  };
  double DEFAULT_almostLogOfProbabilityOf(const SampleType & sample) const {
    const double prob = propToProbabilityOf(sample);
    if (prob==0) {
      throw Lester::LogarithmicTools::LogOfZero();
    };
    const double logProb = log(prob);
    if (!finite(logProb)) {
      throw Lester::LogarithmicTools::LogOfZero();      
    };
    return logProb;
  };
};

template<class SampleType>
inline std::ostream&operator<<(std::ostream & os, const ProbabilityModule<SampleType> & m) {
  return m.printMeTo(os);
};

#endif

