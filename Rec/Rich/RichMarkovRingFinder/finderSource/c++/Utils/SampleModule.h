
#ifndef SAMPLEMODULEH
#define SAMPLEMODULEH

// fwd dec
#include "Utils/SampleModule.fwd" 

// includes

// declaration
template<class SampleType>
struct SampleModule {
  virtual ~SampleModule() {};
  virtual SampleType sample() const = 0;
};


#endif

