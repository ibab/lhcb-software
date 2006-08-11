
#ifndef LESTER_SMALL2VECTOR_H
#define LESTER_SMALL2VECTOR_H

// fwd dec
#include "Small2Vector.fwd" 

// includes
#include <iostream>

// declaration
class Small2Vector {
// Small2Vector() {};
// Small2Vector(const Small2Vector & other) {};
// Small2Vector & operator=(const Small2Vector & other) { return *this; };
// virtual ~Small2Vector() {};


public:
  Small2Vector(const double x, const double y) : m_x(x), m_y(y) {}
  inline double x() const { return m_x; }
  inline double y() const { return m_y; }
  inline Small2Vector operator+(const Small2Vector & other) const {
    return Small2Vector(m_x + other.m_x,
			m_y + other.m_y);
  }
  inline Small2Vector operator-(const Small2Vector & other) const {
    return Small2Vector(m_x - other.m_x,
			m_y - other.m_y);
  }
  inline Small2Vector operator*(const double divisor) const {
    return Small2Vector(m_x*divisor,
			m_y*divisor);
  }
  inline Small2Vector operator/(const double divisor) const {
    return Small2Vector(m_x/divisor,
			m_y/divisor);
  }
  inline double mag2() const {
    return 
      m_x*m_x + 
      m_y*m_y;
  }
  inline double mag() const {
    return sqrt(mag2());
  }
  inline double dot(const Small2Vector & other) const {
    return
      m_x*other.m_x +
      m_y*other.m_y;
  }
private:
  double m_x;
  double m_y;

public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "Small2Vector[]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const Small2Vector & obj) {
  return obj.printMeTo(os);
};

#endif

