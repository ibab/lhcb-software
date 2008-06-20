
#ifndef LESTER_GENERICRING_H
#define LESTER_GENERICRING_H

// fwd dec
#include "GenericRingFinder/GenericRing.fwd"

// includes
#include <iostream>
#include "GenericRingFinder/GenericRingIndex.h"

namespace GenRingF
{

  // declaration
  class GenericRing
  {

  private:
    GenericRing(); // don't want default constructor
    GenericRingIndex m_index;
    double m_x;
    double m_y;
    double m_radius;
  public:
    GenericRing ( const GenericRingIndex index,
                  const double x,
                  const double y,
                  const double radius )
      : m_index(index),
        m_x(x),
        m_y(y),
        m_radius(radius) { }
    inline double x() const { return m_x; };
    inline double y() const { return m_y; };
    inline double radius() const { return m_radius; };
    inline bool operator< (const GenericRing & other) const; // so that rings can be put into maps!
    inline std::ostream & printMeTo(std::ostream & os) const
    {
      os << "GenericRing[index="<< (m_index) <<", x="<<m_x<<", y="<<m_y<<", radius="<<m_radius<<"]";
      return os;
    };
  };

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericRing & obj) {
  return obj.printMeTo(os);
};

#endif

