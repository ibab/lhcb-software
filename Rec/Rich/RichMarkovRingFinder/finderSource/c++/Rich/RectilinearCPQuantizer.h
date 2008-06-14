
#ifndef LESTER_RECTILINEARCPQUANTIZER_H
#define LESTER_RECTILINEARCPQUANTIZER_H

// fwd dec
#include "RectilinearCPQuantizer.fwd"

// includes
#include <iostream>
#include "RegularCPQuantizer.h"

namespace Lester
{

  // declaration
  class RectilinearCPQuantizer : public RegularCPQuantizer
  {

  public:

    RectilinearCPQuantizer ( const double circleMeanRadiusParameter,
                             const double fractionOfMeanRadius = 0.1 )
      : sx(1.0/(fractionOfMeanRadius*circleMeanRadiusParameter)),
        sy(1.0/(fractionOfMeanRadius*circleMeanRadiusParameter)),
        sr(1.0/(fractionOfMeanRadius*circleMeanRadiusParameter)),
        invBVol(sx*sy*sr) { }

  public:

    /// implementation of one of the pure virtual methods required by the base class:
    double inverseBoxVolume() const { return invBVol; }

    /// implementation of one of the pure virtual methods required by the base class
    QuantizedCircleParams quantize(const Lester::CircleParams & cp) const;

  public:

    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "RectilinearCPQuantizer[]";
    }
    friend inline std::ostream & operator<<(std::ostream & os,
                                            const RectilinearCPQuantizer & obj)
    {
      return obj.printMeTo(os);
    }

  private:

    const double sx;
    const double sy;
    const double sr;
    const double invBVol;

  };

}

#endif

