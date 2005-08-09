
#ifndef LESTER_GENERICRING_H
#define LESTER_GENERICRING_H

// fwd dec
#include "GenericRingFinder/GenericRing.fwd" 

// includes
#include <iostream>

namespace GenRingF {

// declaration
class GenericRing {
  GenericRing() {}; // don't want default constructor
// GenericRing(const GenericRing & other) {};
// GenericRing & operator=(const GenericRing & other) { return *this; };
// virtual ~GenericRing() {};
  unsigned int m_index;
  double m_x;
  double m_y;
  double m_radius;
public:
  GenericRing(const unsigned int index,
	      const double x,
	      const double y,
	      const double radius)
    : m_index(index),
      m_x(x),
      m_y(y),
      m_radius(radius) {
  };
  double x() const { return m_x; }; 
  double y() const { return m_y; };
  double radius() const { return m_radius; }; // Yes! Public!
  std::ostream & printMeTo(std::ostream & os) const {
    os << "GenericRing[index="<<m_index<<", x="<<m_x<<", y="<<m_y<<", radius="<<m_radius<<"]";
    return os;
  };
};

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericRing & obj) {
  return obj.printMeTo(os);
};

#endif

