#ifndef ZEMACH_SPIN_2_HH
#define ZEMACH_SPIN_2_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "TLorentzVector.h"
#include "Mint/SymmLorentzMatrix.h"
#include "Mint/ZTspin1.h"
#include <iostream>

class ZTspin2 : public SymmLorentzMatrix{
 public:
  // in decay D -> AB:   q = p(A) - p(B), p = p(A) + p(B)
  ZTspin2(const TLorentzVector& q, const TLorentzVector& p, double mR){
    ZTspin1 t(q, p, mR);
    SymmLorentzMatrix tt(t);
    SymmLorentzMatrix uu(p);
    uu /= (mR * mR);

    *this = tt - (1./3.) * t.Mag2()*(SymmLorentzMatrix::gmunu() - uu);
    // eq 6 in PhysRevD.51.2247
  }
  
  ZTspin2& operator=(const SymmLorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other.v(i);
    return *this;
  }
   
};

#endif
//
