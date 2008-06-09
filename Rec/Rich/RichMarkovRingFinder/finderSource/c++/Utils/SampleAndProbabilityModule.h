
#ifndef SAMPLEANDPROBABILITYMODULEH
#define SAMPLEANDPROBABILITYMODULEH

// fwd dec
#include "Utils/SampleAndProbabilityModule.fwd"

// includes
#include "Utils/SampleModule.h"
#include "Utils/ProbabilityModule.h"

namespace Lester
{

  // declaration
  template<class SampleType>
  struct SampleAndProbabilityModule :
    public SampleModule<SampleType>,
    public ProbabilityModule<SampleType> { };

}

#endif

