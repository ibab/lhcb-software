
#ifndef LESTER_GENERICRINGFINDER_H
#define LESTER_GENERICRINGFINDER_H

// fwd dec
#include "GenericRingFinder/GenericRingFinder.fwd" 

// includes
#include <iostream>
#include <exception>
#include "GenericRingFinder/GenericInput.h"
#include "GenericRingFinder/GenericResults.h"

namespace GenRingF {

// declaration
class GenericRingFinder {
// virtual ~GenericRingFinder() {};
public:
  class CouldNotFit : public std::exception {
    const char * what() const throw() { 
      return "CouldNotFit";
    };
  };
  virtual GenericResults fit(const GenericInput & input) throw (CouldNotFit) = 0;
};

}

#endif

