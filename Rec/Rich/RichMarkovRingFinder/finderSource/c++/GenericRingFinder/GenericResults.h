
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
#include "Utils/OstreamUtils.h"

namespace GenRingF {

  // declaration
  class GenericResults {
  public:
    GenericResults() : meanBackground(0), numIterations(0), timeTaken(0) {}
  public:
    typedef std::vector<GenericRing> GenericRings;
    GenericRings rings;
    boost::shared_ptr<GenericInferrer> inferrer;
    double meanBackground;
    unsigned int numIterations; ///< Number of iterations taken
    double timeTaken;           ///< Time taken in ms
  public:
    std::ostream & printMeTo(std::ostream & os) const {
      os << "GenericResults[ meanBackground=" << meanBackground << " numIts=" << numIterations 
         << " time=" << timeTaken << " " << rings << " ]";
      return os;
    };
  };

}

inline std::ostream & operator<< ( std::ostream & os, 
                                   const GenRingF::GenericResults & obj ) 
{
  return obj.printMeTo(os);
}

#endif

