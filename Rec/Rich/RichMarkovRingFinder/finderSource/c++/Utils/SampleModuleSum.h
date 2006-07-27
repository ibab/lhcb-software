
#ifndef SAMPLEMODULESUMH
#define SAMPLEMODULESUMH

// fwd dec
#include "Utils/SampleModuleSum.fwd" 

// includes
#include "Utils/SampleModule.h"
#include "boost/shared_ptr.hpp"

// declaration
template<class SampleType>
struct SampleModuleSum : public SampleModule<SampleType> {
private:
  boost::shared_ptr<const SampleModule<SampleType> > m_distA;
  boost::shared_ptr<const SampleModule<SampleType> > m_distB;
  const double m_aWeight;
  const double m_bWeight;
public:
  class BadWeight {};
  class BadProbabilityModule {};
  SampleModuleSum(boost::shared_ptr<const SampleModule<SampleType> > distA,
		  boost::shared_ptr<const SampleModule<SampleType> > distB,
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
  virtual SampleType sample() const {
    if (RandFlat::shoot()<m_aWeight) {
      return m_distA->sample();
    } else {
      return m_distB->sample();
    };
  };
  virtual ~SampleModuleSum() {};
};


#endif

