
#ifndef LESTER_GENERICRESULTS_H
#define LESTER_GENERICRESULTS_H

// fwd dec
#include "GenericRingFinder/GenericResults.fwd" 

// includes
#include <iostream>
#include <list>
#include "GenericRingFinder/GenericRing.h"

namespace GenRingF {

// declaration
class GenericResults {
// GenericResults() {};
// GenericResults(const GenericResults & other) {};
// GenericResults & operator=(const GenericResults & other) { return *this; };
// virtual ~GenericResults() {};
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "GenericResults[]";
    return os;
  };
  std::list<GenericRing> rings;
  double meanBackground;
};

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericResults & obj) {
  return obj.printMeTo(os);
};

#endif

