
#ifndef FLATDISTRIBUTIONH
#define FLATDISTRIBUTIONH

// fwd dec
#include "Utils/FlatDistribution.fwd"

// includes
#include <iostream>
#include "Utils/SampleAndProbabilityModule.h"
#include "CLHEP/Random/Randomize.h"
#include "Utils/CannotConstructException.h"

namespace Lester
{

  // declaration
  struct FlatDistribution : public SampleAndProbabilityModule<double> 
  {
    const double m_from;
    const double m_to;
    const double m_width;
    const double m_oneOnWidth;
    explicit FlatDistribution(const double from=0, const double to=1) :
      m_from(from),
      m_to(to),
      m_width(to-from),
      m_oneOnWidth(1./m_width) 
    {
      if (m_width<=0) 
      {
        throw CannotConstructException("Tried to create FlatDistribution with from>=to.");
      }
    }
    double sample() const
    {
      return RandFlat::shoot(m_from, m_to);
    }
    double probabilityOf(const double & d) const
    {
      return ( d>m_from && d<m_to ? m_oneOnWidth : 0 );
    }
    double propToProbabilityOfValidPoint() const
    {
      return 1;
    }
    double probabilityOfValidPoint() const
    {
      return m_oneOnWidth;
    }
  public:
    std::ostream & printMeTo(std::ostream & os) const {
      return os << "FlatDistribution[from="<<m_from<<", to="<<m_to<<"]";
    }
  };

  inline std::ostream & operator<<(std::ostream & os, const FlatDistribution & obj)
  {
    return obj.printMeTo(os);
  }

}

#endif

