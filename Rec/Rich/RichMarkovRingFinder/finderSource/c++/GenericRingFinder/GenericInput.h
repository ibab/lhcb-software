
#ifndef LESTER_GENERICINPUT_H
#define LESTER_GENERICINPUT_H

// fwd dec
#include "GenericRingFinder/GenericInput.fwd" 

// includes
#include <iostream>
#include <vector>
#include "GenericRingFinder/GenericHit.h"

namespace GenRingF {

// declaration
class GenericInput {
// GenericInput() {};
// GenericInput(const GenericInput & other) {};
// GenericInput & operator=(const GenericInput & other) { return *this; };
// virtual ~GenericInput() {};
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "GenericInput[]";
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

