
#ifndef LESTER_RECTILINEARCPQUANTIZER_H
#define LESTER_RECTILINEARCPQUANTIZER_H

// fwd dec
#include "RectilinearCPQuantizer.fwd"

// includes
#include <iostream>
#include "RegularCPQuantizer.h"
#include "NimTypeRichModel.fwd"

namespace Lester
{

  // declaration
  class RectilinearCPQuantizer : public RegularCPQuantizer 
  {
  public:
    RectilinearCPQuantizer(const Lester::NimTypeRichModel & ntrm, 
                           const double fractionOfMeanRadius=0.1);
  private:
    const double sx;
    const double sy;
    const double sr;
    const double invBVol;
  public:
    /// implementation of oneof the pure virtual methods required by the base class:
    double inverseBoxVolume() const { return invBVol; }
    /// implementation of oneof the pure virtual methods required by the base class
    QuantizedCircleParams quantize(const Lester::CircleParams & cp) const;
    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "RectilinearCPQuantizer[]";
    }
    friend inline std::ostream & operator<<(std::ostream & os,
                                            const RectilinearCPQuantizer & obj)
    {
      return obj.printMeTo(os);
    }
  };

}

#endif

