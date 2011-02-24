#ifndef PHASE_HH
#define PHASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <iostream>

namespace MINT{
class Phase{
 protected:
  double _ph;
  const static double _minPh;
  void toRange();
 public:
  Phase(const Phase& other);
  Phase(double ph_in=0);
  operator double() const{
    return _ph;
  }

  static double rangeMax();
  static double rangeMin();

  Phase& operator*=(double rhs);
  Phase& operator/=(double rhs);
  Phase& operator+=(const Phase& rhs);
  Phase  operator+ (const Phase& rhs) const;
  const Phase&  operator+ () const;
  Phase& operator-=(const Phase& rhs);
  Phase  operator- (const Phase& rhs) const;
  Phase  operator- () const;

  double inDegrees() const;
  void testPhase(std::ostream& os = std::cout) const;
};
}//namespace MINT

#endif
