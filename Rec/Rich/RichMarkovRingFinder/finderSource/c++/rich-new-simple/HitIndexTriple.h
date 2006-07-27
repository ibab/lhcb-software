
#ifndef LESTER_HITINDEXTRIPLE_H
#define LESTER_HITINDEXTRIPLE_H

// fwd dec
#include "HitIndexTriple.fwd" 

// includes
#include <iostream>

// declaration
class HitIndexTriple {
public:
  int a;
  int b;
  int c;
public:
  HitIndexTriple(const int aa,
		 const int bb,
		 const int cc) : a(aa), b(bb), c(cc) {};
  HitIndexTriple(const HitIndexTriple & other) : 
    a(other.a),
    b(other.b),
    c(other.c) {
  };
  HitIndexTriple & operator=(const HitIndexTriple & other) {
    a = other.a;
    b = other.b;
    c = other.c;
    return *this;
  };
  virtual ~HitIndexTriple() {};
  bool operator<(const HitIndexTriple & other) const {

    if (a < other.a) {
      return true;
    } else if (a > other.a) {
      return false;
    } else {
     
      if (b < other.b) {
	return true;
      } else if (b > other.b) {
	return false;
      } else {

	return (c < other.c);

      };

    };
  };
public:
  std::ostream & printMeTo(std::ostream & os) const {
    os << "HitIndexTriple["<<a<<","<<b<<","<<c<<"]";
    return os;
  };
};

inline std::ostream & operator<<(std::ostream & os, const HitIndexTriple & obj) {
  return obj.printMeTo(os);
};

#endif

