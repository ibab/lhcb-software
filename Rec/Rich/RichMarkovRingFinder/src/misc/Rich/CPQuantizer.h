
#ifndef LESTER_CPQUANTIZER_H
#define LESTER_CPQUANTIZER_H

// fwd dec
#include "CPQuantizer.fwd"

// includes
#include <iostream>
#include "Utils/Quantizer.h"
#include "CircleParams.h"
#include "QuantizedCircleParams.h"

namespace Lester
{

  // declaration
  class CPQuantizer : public Quantizer<Lester::CircleParams,
                      QuantizedCircleParams>
  {
    // CPQuantizer() {};
    // CPQuantizer(const CPQuantizer & other) {};
    // CPQuantizer & operator=(const CPQuantizer & other) { return *this; };
    // virtual ~CPQuantizer() {};
  public:
    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "CPQuantizer[]";
    }
    friend inline std::ostream & operator<<(std::ostream & os, const CPQuantizer & obj)
    {
      return obj.printMeTo(os);
    }
  };

}

#endif

