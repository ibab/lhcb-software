
#ifndef LESTER_REGULARCPQUANTIZER_H
#define LESTER_REGULARCPQUANTIZER_H

// fwd dec
#include "RegularCPQuantizer.fwd" 

// includes
#include <iostream>
#include "CPQuantizer.h"

// declaration
class RegularCPQuantizer : public CPQuantizer {
// RegularCPQuantizer() {};
// RegularCPQuantizer(const RegularCPQuantizer & other) {};
// RegularCPQuantizer & operator=(const RegularCPQuantizer & other) { return *this; };
// virtual ~RegularCPQuantizer() {};
public:
  virtual double inverseBoxVolume() const = 0;
  std::ostream & printMeTo(std::ostream & os) const {
    os << "RegularCPQuantizer[]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const RegularCPQuantizer & obj) {
  return obj.printMeTo(os);
};

#endif

