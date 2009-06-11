
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

#ifdef _WIN32
#pragma warning ( disable:4290 ) // Visual C++ does not implement checked exceptions
#endif

namespace GenRingF
{

  // declaration
  class GenericRingFinder
  {
  public:
    class CouldNotFit : public std::exception 
    {
    public:
      CouldNotFit( const std::string & mess = "Undefined Message" )
        : m_message( "CouldNotFit[ "+mess+" ]" ) { }
      const char * what() const throw() 
      {
        return m_message.c_str();
      }
      virtual ~CouldNotFit() throw() {}
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
