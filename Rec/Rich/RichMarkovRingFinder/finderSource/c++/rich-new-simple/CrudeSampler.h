
#ifndef LESTER_CRUDESAMPLER_H
#define LESTER_CRUDESAMPLER_H

// fwd dec
#include "CrudeSampler.fwd"

// includes
#include <iostream>
#include "GenericRingFinder/GenericRingFinder.h"
#include "GenericRingFinder/GenericResults.fwd"
#include "RichParams.fwd"
#include "ThreePointCircleProposerB.fwd"
#include "MyRichMetropolisSampler.fwd"
#include "Data.fwd"

// declaration
class CrudeSampler : public GenRingF::GenericRingFinder {
public:
  CrudeSampler();
  virtual boost::shared_ptr<GenRingF::GenericResults> fit(const GenRingF::GenericInput & input) throw (CouldNotFit);
  std::ostream & printMeTo(std::ostream & os) const;
private:
  void doTheWork( Lester::RichParams & currentPoint,
                  double & currentLogProb,
                  Lester::ThreePointCircleProposerB & p,
                  Lester::MyRichMetropolisSampler & sampler,
                  const Lester::Data & data);
};

std::ostream & operator<<(std::ostream & os, const CrudeSampler & obj);

#endif

