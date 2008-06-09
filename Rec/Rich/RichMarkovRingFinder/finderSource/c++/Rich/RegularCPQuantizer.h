
#ifndef LESTER_REGULARCPQUANTIZER_H
#define LESTER_REGULARCPQUANTIZER_H

// fwd dec
#include "RegularCPQuantizer.fwd"

// includes
#include <iostream>
#include "CPQuantizer.h"

namespace Lester
{

  // declaration
  class RegularCPQuantizer : public CPQuantizer {
    // RegularCPQuantizer() {};
    // RegularCPQuantizer(const RegularCPQuantizer & other) {};
    // RegularCPQuantizer & operator=(const RegularCPQuantizer & other) { return *this; };
    // virtual ~RegularCPQuantizer() {};
  public:
    virtual double inverseBoxVolume() const = 0;
    std::ostream & printMeTo(std::ostream & os) const 
    {
      return os << "RegularCPQuantizer[]";
    }
    friend inline std::ostream & operator<<(std::ostream & os,
                                            const RegularCPQuantizer & obj)
    {
      return obj.printMeTo(os);
    }

  };

}

#endif
