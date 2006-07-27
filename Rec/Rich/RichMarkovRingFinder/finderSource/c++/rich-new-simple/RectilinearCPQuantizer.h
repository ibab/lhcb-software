
#ifndef LESTER_RECTILINEARCPQUANTIZER_H
#define LESTER_RECTILINEARCPQUANTIZER_H

// fwd dec
#include "RectilinearCPQuantizer.fwd" 

// includes
#include <iostream>
#include "RegularCPQuantizer.h"
#include "Constants.h"

// declaration
class RectilinearCPQuantizer : public RegularCPQuantizer {
public:
  RectilinearCPQuantizer(const double fractionOfMeanRadius=0.1) :
    sx(1./(fractionOfMeanRadius*Lester::Constants::circleMeanRadiusParameter)),
    sy(1./(fractionOfMeanRadius*Lester::Constants::circleMeanRadiusParameter)),
    sr(1./(fractionOfMeanRadius*Lester::Constants::circleMeanRadiusParameter)),
    invBVol(sx*sy*sr) {
  };
// RectilinearCPQuantizer(const RectilinearCPQuantizer & other) {};
// RectilinearCPQuantizer & operator=(const RectilinearCPQuantizer & other) { return *this; };
// virtual ~RectilinearCPQuantizer() {};
private:
  const double sx;
  const double sy;
  const double sr;
  const double invBVol;
public:
  /// implementation of oneof the pure virtual methods required by the base class:
  double inverseBoxVolume() const {
    return invBVol;
  };
  /// implementation of oneof the pure virtual methods required by the base class
  QuantizedCircleParams quantize(const Lester::CircleParams & cp) const;
  std::ostream & printMeTo(std::ostream & os) const {
    os << "RectilinearCPQuantizer[]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const RectilinearCPQuantizer & obj) {
  return obj.printMeTo(os);
};

#endif

