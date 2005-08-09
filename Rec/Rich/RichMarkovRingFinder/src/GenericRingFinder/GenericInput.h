
#ifndef LESTER_GENERICINPUT_H
#define LESTER_GENERICINPUT_H

// fwd dec
#include "GenericRingFinder/GenericInput.fwd" 

// includes
#include <iostream>
#include <list>
#include "GenericRingFinder/GenericHit.h"
#include "GenericRingFinder/GenericRing.h"

namespace GenRingF {

// declaration
class GenericInput {
// GenericInput() {};
// GenericInput(const GenericInput & other) {};
// GenericInput & operator=(const GenericInput & other) { return *this; };
// virtual ~GenericInput() {};
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "GenericInput[Hits=";
    for (std::list<GenericHit>::const_iterator it = hits.begin();
	 it != hits.end();
	 ++it) {
      os << *it <<",";
    };
    os << " MCRings=";
    for (std::list<GenericRing>::const_iterator it = ringsFromMonteCarlo.begin();
	 it != ringsFromMonteCarlo.end();
	 ++it) {
      os << *it <<",";
    };
    os <<"]";
    return os;
  };
  std::list<GenericHit> hits; // The hits that you want to fit
  std::list<GenericRing> ringsFromMonteCarlo; // You can leave this empty (as it would be in the real experiment!) or fill it with montecarlo truth rings that you might like to see plotted alongside hits for debugging purposes ...
};

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericInput & obj) {
  return obj.printMeTo(os);
};

#endif

