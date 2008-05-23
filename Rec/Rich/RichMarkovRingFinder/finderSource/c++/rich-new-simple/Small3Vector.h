
#ifndef LESTER_SMALL3VECTOR_H
#define LESTER_SMALL3VECTOR_H

// fwd dec
#include "Small3Vector.fwd" 

// includes
#include <iostream>
#include <cmath>

// declaration
class Small3Vector {
// Small3Vector() {};
// Small3Vector(const Small3Vector & other) {};
// Small3Vector & operator=(const Small3Vector & other) { return *this; };
// virtual ~Small3Vector() {};

public:
  Small3Vector(const double x, const double y, const double z) : m_x(x), m_y(y), m_z(z) {}
  inline double x() const { return m_x; }
  inline double y() const { return m_y; }
  inline double z() const { return m_z; }
  inline Small3Vector operator+(const Small3Vector & other) const {
    return Small3Vector(m_x + other.m_x,
			m_y + other.m_y,
			m_z + other.m_z);
  }
  inline Small3Vector operator-(const Small3Vector & other) const {
    return Small3Vector(m_x - other.m_x,
			m_y - other.m_y,
			m_z - other.m_z);
  }
  inline Small3Vector operator*(const double divisor) const {
    return Small3Vector(m_x*divisor,
			m_y*divisor,
			m_z*divisor);
  }
  inline Small3Vector operator/(const double divisor) const {
    return Small3Vector(m_x/divisor,
			m_y/divisor,
			m_z/divisor);
  }
  inline double mag2() const {
    return 
      m_x*m_x + 
      m_y*m_y + 
      m_z*m_z;
  }
  inline double mag() const {
    return sqrt(mag2());
  }
  inline double dot(const Small3Vector & other) const {
    return
      m_x*other.m_x +
      m_y*other.m_y +
      m_z*other.m_z;
  }
  inline Small3Vector cross(const Small3Vector & other) const {
    return Small3Vector(m_y*other.m_z - m_z*other.m_y,
			m_z*other.m_x - m_x*other.m_z,
			m_x*other.m_y - m_y*other.m_x);
  }
private:
  double m_x;
  double m_y;
  double m_z;


public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "Small3Vector[]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const Small3Vector & obj) {
  return obj.printMeTo(os);
};

#endif

