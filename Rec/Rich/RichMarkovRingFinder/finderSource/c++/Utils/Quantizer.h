
#ifndef LESTER_QUANTIZER_H
#define LESTER_QUANTIZER_H

// fwd dec
#include "Utils/Quantizer.fwd" 

// includes
#include <iostream>

// declaration
template<class Unquantized, class Quantized>
class Quantizer {
// Quantizer() {};
// Quantizer(const Quantizer & other) {};
// Quantizer & operator=(const Quantizer & other) { return *this; };
public:
  virtual ~Quantizer() {};
  virtual Quantized quantize(const Unquantized & other) const = 0;
  std::ostream & printMeTo(std::ostream & os) const {
    os << "Quantizer[]";
    return os;
  };
};

template<class Unquantized, class Quantized>
inline std::ostream & operator<<(std::ostream & os, const Quantizer<Unquantized, Quantized> & obj) {
  return obj.printMeTo(os);
};

#endif

