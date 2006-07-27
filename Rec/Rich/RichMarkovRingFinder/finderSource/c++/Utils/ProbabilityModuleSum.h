
#ifndef PROBABILITYMODULESUMH
#define PROBABILITYMODULESUMH

// fwd dec
#include "Utils/ProbabilityModuleSum.fwd" 

// includes
#include "Utils/LogarithmicTools.h"
#include "Utils/ProbabilityModule.h"
#include "boost/shared_ptr.hpp"

// declaration
template<class SampleType>
struct ProbabilityModuleSum : public ProbabilityModule<SampleType> {
private:
  boost::shared_ptr<const ProbabilityModule<SampleType> > m_distA;
  boost::shared_ptr<const ProbabilityModule<SampleType> > m_distB;
  const double m_aWeight;
  const double m_bWeight;
public:
  class BadWeight {};
  class BadProbabilityModule {};
  ProbabilityModuleSum(boost::shared_ptr<const ProbabilityModule<SampleType> > distA,
		       boost::shared_ptr<const ProbabilityModule<SampleType> > distB,
		       const double aWeight=1,
		       const double bWeight=1) :
    m_distA(distA),
    m_distB(distB),
    m_aWeight(fabs(aWeight)/(fabs(aWeight)+fabs(bWeight))),
    m_bWeight(fabs(bWeight)/(fabs(aWeight)+fabs(bWeight))) {
    if (!finite(fabs(aWeight)+fabs(bWeight))) {
      throw BadWeight();
    };
    if ((!distA)||(!distB)) {
      throw BadProbabilityModule();
    };
  };
  virtual ~ProbabilityModuleSum() {};

  virtual double probabilityOf(const SampleType & sample) const {
    return 
      m_distA->probabilityOf(sample)*m_aWeight +
      m_distB->probabilityOf(sample)*m_bWeight ;
  };
  virtual double propToProbabilityOf(const SampleType & sample) const {
    return probabilityOf(sample);
  };
  virtual double logOfProbabilityOf(const SampleType & sample) const {
    Lester::LogarithmicTools::IncrementalAdder adder;
    try {
      adder.addNumberWhoseLogIs(m_distA->logOfProbabilityOf(sample) + log(m_aWeight));
    } catch(Lester::LogarithmicTools::LogOfZero) {
      // do nothing
    };
    try {
      adder.addNumberWhoseLogIs(m_distB->logOfProbabilityOf(sample) + log(m_bWeight));
    } catch(Lester::LogarithmicTools::LogOfZero) {
      // do nothing
    };
    return adder.logOfTotal();
  };
  virtual double almostLogOfProbabilityOf(const SampleType & sample) const {
    return logOfProbabilityOf(sample);
  };

};



#endif

