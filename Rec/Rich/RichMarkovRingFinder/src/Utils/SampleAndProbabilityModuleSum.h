
#ifndef SAMPLEANDPROBABILITYMODULESUMH
#define SAMPLEANDPROBABILITYMODULESUMH

// fwd dec
#include "Utils/SampleAndProbabilityModuleSum.fwd" 

// includes
#include "Utils/SampleModuleSum.h"
#include "Utils/ProbabilityModuleSum.h"
#include "Utils/SampleAndProbabilityModule.h"

// declaration
template<class SampleType>
struct SampleAndProbabilityModuleSum :
  public SampleAndProbabilityModule<SampleType> {
private: 
  SampleModuleSum     <SampleType> m_sampleModule;
  ProbabilityModuleSum<SampleType> m_probModule;
public:
  SampleAndProbabilityModuleSum(boost::shared_ptr<const SampleAndProbabilityModule<SampleType> > distA,
				boost::shared_ptr<const SampleAndProbabilityModule<SampleType> > distB,
				const double aWeight=1,
				const double bWeight=1) :
    m_sampleModule(distA, distB, aWeight, bWeight),
    m_probModule  (distA, distB, aWeight, bWeight) {
  };
  double logOfProbabilityOf(const SampleType & sample) const {
    return m_probModule.logOfProbabilityOf(sample);
  };
  double almostLogOfProbabilityOf(const SampleType & sample) const {
    return m_probModule.almostLogOfProbabilityOf(sample);
  };
  double probabilityOf(const SampleType & sample) const {
    return m_probModule.probabilityOf(sample);
  };
  double propToProbabilityOf(const SampleType & sample) const {
    return m_probModule.propToProbabilityOf(sample);
  };
  SampleType sample() const {
    return m_sampleModule.sample();
  };
};


#endif

