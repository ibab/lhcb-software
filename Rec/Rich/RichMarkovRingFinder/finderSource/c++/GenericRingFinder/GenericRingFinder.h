
#ifndef LESTER_GENERICRINGFINDER_H
#define LESTER_GENERICRINGFINDER_H

// fwd dec
#include "GenericRingFinder/GenericRingFinder.fwd"

// includes
#include <string>
#include <iostream>
#include <exception>
#include "GenericRingFinder/GenericInput.h"
#include "GenericRingFinder/GenericResults.h"
#include "GenericRingFinder/GenericConfiguration.h"
#include "boost/shared_ptr.hpp"

namespace GenRingF
{

  // declaration
  class GenericRingFinder
  {
  public:
    class CouldNotFit : public std::exception {
    public:
      CouldNotFit( const std::string & mess =  "ErDunno" )
        : m_message(mess) { }
      const char * what() const throw() {
        const std::string m = "CouldNotFit["+m_message+"]";
        return m.c_str();
      };
      virtual ~CouldNotFit() throw() {};
    private:
      std::string m_message;
    };
  public:
    virtual ~GenericRingFinder() { }
    GenericConfiguration configuration;
    virtual boost::shared_ptr<GenericResults> fit(const GenericInput & input) throw (CouldNotFit) = 0;
  };

}

#endif
