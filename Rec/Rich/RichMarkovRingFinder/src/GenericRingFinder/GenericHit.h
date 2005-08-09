
#ifndef LESTER_GENERICHIT_H
#define LESTER_GENERICHIT_H

// fwd dec
#include "GenericRingFinder/GenericHit.fwd" 

// includes
#include <iostream>

namespace GenRingF {

// declaration
class GenericHit {
  GenericHit() {}; // don't want default constructor
// GenericHit(const GenericHit & other) {};
// GenericHit & operator=(const GenericHit & other) { return *this; };
// virtual ~GenericHit() {};
  unsigned int m_index;
  double m_x;
  double m_y;
public:
  GenericHit(unsigned int index,
	     const double x,
	     const double y) 
    : m_index(index),
      m_x(x),
      m_y(y) {
  };
  double x() const { return m_x; };
  double y() const { return m_y; };
  std::ostream & printMeTo(std::ostream & os) const {
    os << "GenericHit[index="<<m_index<<", x="<<m_x<<", y="<<m_y<<"]";
    return os;
  };
};

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericHit & obj) {
  return obj.printMeTo(os);
};

#endif

