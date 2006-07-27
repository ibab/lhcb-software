
#ifndef LESTER_GENERICINPUT_H
#define LESTER_GENERICINPUT_H

// fwd dec
#include "GenericRingFinder/GenericInput.fwd" 

// includes
#include <iostream>
#include <list>
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
  std::list<GenericHit> hits;
};

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericInput & obj) {
  return obj.printMeTo(os);
};

#endif

