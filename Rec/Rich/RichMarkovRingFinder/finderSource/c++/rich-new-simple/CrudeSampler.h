
#ifndef LESTER_CRUDESAMPLER_H
#define LESTER_CRUDESAMPLER_H

// fwd dec
#include "CrudeSampler.fwd"

// includes
#include <iostream>
#include <list>
#include "GenericRingFinder/GenericRingFinder.h"
#include "Data.h"
#include "ThreePointCircleProposerB.h"
#include "RectilinearCPQuantizer.h"
#include "Constants.h"
#include "RichParams.h"
#include "MyRichMetropolisSampler.h"
#include "GraphicsObjects.h"

// declaration
class CrudeSampler : public GenRingF::GenericRingFinder {
  // CrudeSampler() {};
  // CrudeSampler(const CrudeSampler & other) {};
  // CrudeSampler & operator=(const CrudeSampler & other) { return *this; };
  // virtual ~CrudeSampler() {};
public:
  CrudeSampler() {
  };
  virtual GenRingF::GenericResults fit(const GenRingF::GenericInput & input) throw (CouldNotFit);
  std::ostream & printMeTo(std::ostream & os) const {
    os << "CrudeSampler[]";
    return os;
  };
private:
  void doTheWork( Lester::RichParams & currentPoint, 
                  double & currentLogProb,
                  Lester::ThreePointCircleProposerB & p,
                  Lester::MyRichMetropolisSampler & sampler,
		  const Lester::Data & data);
};

inline std::ostream & operator<<(std::ostream & os, const CrudeSampler & obj) {
  return obj.printMeTo(os);
};

#endif

