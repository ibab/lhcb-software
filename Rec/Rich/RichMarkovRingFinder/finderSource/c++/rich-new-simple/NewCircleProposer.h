
#ifndef LESTER_NEWCIRCLEPROPOSER_H
#define LESTER_NEWCIRCLEPROPOSER_H

// fwd dec
#include "NewCircleProposer.fwd" 

// includes
#include <iostream>
#include "CircleParams.h"
#include "Utils/SampleAndProbabilityModule.h"

// declaration
namespace Lester {

class NewCircleProposer : public SampleAndProbabilityModule<CircleParams> {
// NewCircleProposer() {};
// NewCircleProposer(const NewCircleProposer & other) {};
// NewCircleProposer & operator=(const NewCircleProposer & other) { return *this; };
// virtual ~NewCircleProposer() {};
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "NewCircleProposer[]";
    return os;
  };
  double propToProbabilityOf(const CircleParams & sample) const {
    return probabilityOf(sample);
  };
   double logOfProbabilityOf(const CircleParams & sample) const {
	return DEFAULT_logOfProbabilityOf(sample);
  };
  double almostLogOfProbabilityOf(const CircleParams & sample) const {
	return DEFAULT_almostLogOfProbabilityOf(sample);
  }; 
};

};

inline std::ostream & operator<<(std::ostream & os, const Lester::NewCircleProposer & obj) {
  return obj.printMeTo(os);
};

#endif

