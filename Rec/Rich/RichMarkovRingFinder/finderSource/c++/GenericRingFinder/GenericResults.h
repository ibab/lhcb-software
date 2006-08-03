
#ifndef LESTER_GENERICRESULTS_H
#define LESTER_GENERICRESULTS_H

// fwd dec
#include "GenericRingFinder/GenericResults.fwd"

// includes
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "GenericRingFinder/GenericRing.h"
#include "GenericRingFinder/GenericInferrer.fwd"

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
    typedef std::vector<GenericRing> GenericRings;
    GenericRings rings;
    boost::shared_ptr<GenericInferrer> inferrer;
    double meanBackground;
  };

}

inline std::ostream & operator<< ( std::ostream & os, 
                                   const GenRingF::GenericResults & obj ) 
{
  return obj.printMeTo(os);
}

#endif

