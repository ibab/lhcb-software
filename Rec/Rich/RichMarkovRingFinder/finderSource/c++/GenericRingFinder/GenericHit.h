
#ifndef LESTER_GENERICHIT_H
#define LESTER_GENERICHIT_H

// fwd dec
#include "GenericRingFinder/GenericHit.fwd"

// includes
#include <iostream>
#include "GenericRingFinder/GenericHitIndex.h"

namespace GenRingF {

  // declaration
  class GenericHit {
  private:
    GenericHit(); // don't want default constructor
    GenericHitIndex m_index;
    double m_x;
    double m_y;
  public:
    GenericHit(const GenericHitIndex index,
               const double x,
               const double y)
      : m_index(index),
        m_x(x),
        m_y(y) {
    };
    inline double x() const { return m_x; };
    inline double y() const { return m_y; };
    inline const GenericHitIndex & index() const { return m_index; };
    inline bool operator< (const GenericHit & other) const; // so that hits can be put into maps!
    inline std::ostream & printMeTo(std::ostream & os) const {
      os << "GenericHit[index="<< (m_index) <<", x="<<m_x<<", y="<<m_y<<"]";
      return os;
    };
  };

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericHit & obj) {
  return obj.printMeTo(os);
};

#endif

