#ifndef ZEMACH_SPIN_1_HH
#define ZEMACH_SPIN_1_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "TLorentzVector.h"
//#include <iostream>

class ZTspin1 : public TLorentzVector{
 public:
  // in decay D -> AB:   q = p(A) - p(B), p = p(A) + p(B)
 ZTspin1(const TLorentzVector& q, const TLorentzVector& p, double mR)
   : TLorentzVector(q - q.Dot(p)*p * (1./(mR*mR))){}

  double Contract(const TLorentzVector& rhs) const{
    return this->Dot(rhs);
  }
};

#endif
//
