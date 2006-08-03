
#ifndef LESTER_GENERICRINGFINDER_H
#define LESTER_GENERICRINGFINDER_H

// fwd dec
#include "GenericRingFinder/GenericRingFinder.fwd"

// includes
#include <iostream>
#include <exception>
#include "GenericRingFinder/GenericInput.h"
#include "GenericRingFinder/GenericResults.h"
#include "boost/shared_ptr.hpp"

namespace GenRingF
{

  // declaration
  class GenericRingFinder
  {
  public:
    class CouldNotFit : public std::exception {
    public:
      const char * what() const throw() {
        return "CouldNotFit";
      };
    };
    virtual boost::shared_ptr<GenericResults> fit(const GenericInput & input) throw (CouldNotFit) = 0;
  };

}

#endif
