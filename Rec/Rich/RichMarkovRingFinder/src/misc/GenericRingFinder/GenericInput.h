
#ifndef LESTER_GENERICINPUT_H
#define LESTER_GENERICINPUT_H

// fwd dec
#include "GenericRingFinder/GenericInput.fwd"

// includes
#include <iostream>
#include <vector>
#include "GenericRingFinder/GenericHit.h"
#include "Utils/OstreamUtils.h"

namespace GenRingF {

  // declaration
  class GenericInput {
  public:
    std::ostream & printMeTo(std::ostream & os) const {
      os << "GenericInput[ " << hits << " ]";
      return os;
    };
    typedef std::vector<GenericHit> GenericHits;
    GenericHits hits;
  };

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericInput & obj) {
  return obj.printMeTo(os);
};

#endif

