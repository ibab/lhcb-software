#ifndef DALITZ_AMP_RATIOS_HH
#define DALITZ_AMP_RATIOS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT

#include "DalitzMCIntegrator.h"
#include "DalitzEventPattern.h"
#include "FitAmpSum.h"

class AmpRatios{

  DalitzMCIntegrator _integrator;
  DalitzEventPattern _pattern;

 public:
  AmpRatios(const DalitzEventPattern& pattern);

  bool getRatios(FitAmpSum& ampSum);
};

#endif
//
