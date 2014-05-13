#ifndef ZEMACH_GMUNU_PERPENDICULAR_HH
#define ZEMACH_GMUNU_PERPENDICULAR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "TLorentzVector.h"
#include "Mint/SymmLorentzMatrix.h"
#include <iostream>

class ZTgperp : public SymmLorentzMatrix{
 public:
  ZTgperp(const TLorentzVector& p, double mR_in = -9999){
    // the "true" gmunu_perp should be with u = p/sqrt(p^2)
    // however, often we use instead u = p/m_R where m_R is 
    // the "nominal" mass (at the peak) rather than
    // the (off-shell) mass of the resonance.
    // m_R < 0 uses sqrt(p^2), otherwise it's m_R

    double mR2 = (mR_in < 0 ? p.M2() : mR_in*mR_in);
    SymmLorentzMatrix uu(p);
    uu /= mR2;

    *this = (SymmLorentzMatrix::gmunu() - uu);
  }
  
  ZTgperp& operator=(const SymmLorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other.v(i);
    return *this;
  }
   
};

#endif
//
