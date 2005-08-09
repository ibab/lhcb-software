
#ifndef LESTER_QUANTIZEDCIRCLEPARAMS_H
#define LESTER_QUANTIZEDCIRCLEPARAMS_H

// fwd dec
#include "QuantizedCircleParams.fwd" 

// includes
#include <iostream>

// declaration
/// This class will only make sense or be decoded by a given CPQuantizer.
class QuantizedCircleParams {
public:
  int xBin;
  int yBin;
  int rBin;
  QuantizedCircleParams(const int xB, const int yB, const int rB) :
    xBin(xB),
    yBin(yB),
    rBin(rB) {
  };
  bool operator<(const QuantizedCircleParams & other) const {
    if (xBin < other.xBin) {
      return true;
    };
    if (xBin > other.xBin) {
      return false;
    };
    if (yBin < other.yBin) {
      return true;
    };
    if (yBin > other.yBin) {
      return false;
    };
    return rBin < other.rBin;
  };
  bool operator!=(const QuantizedCircleParams & other) const {
    return 
      xBin != other.xBin ||
      yBin != other.yBin ||
      rBin != other.rBin;
  };
  bool operator==(const QuantizedCircleParams & other) const {
    return 
      xBin == other.xBin &&
      yBin == other.yBin &&
      rBin == other.rBin;
  };
// QuantizedCircleParams(const QuantizedCircleParams & other) {};
// QuantizedCircleParams & operator=(const QuantizedCircleParams & other) { return *this; };
// virtual ~QuantizedCircleParams() {};
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "QuantizedCircleParams[]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const QuantizedCircleParams & obj) {
  return obj.printMeTo(os);
};

#endif

