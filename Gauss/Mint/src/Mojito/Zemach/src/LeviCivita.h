#ifndef LEVI_CIVITA_HH
#define LEVI_CIVITA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "TLorentzVector.h"
#include "TMatrixT.h"
//#include <iostream>

inline double LeviCivita(const TLorentzVector& p0
			 , const TLorentzVector& p1
			 , const TLorentzVector& p2
			 , const TLorentzVector& p3){
  
  // it is the determinant of the matrix made up of p0, p1, p2, p3
  TMatrixT<double> m(4, 4);
  for(int i=0; i < 4; i++){
    // strange indexing, because I have 0th component = E
    // but Rene has 0th component = px, 3rd component = E:
    m(0, i)=p0[(i+3)%4];
    m(1, i)=p1[(i+3)%4];
    m(2, i)=p2[(i+3)%4];
    m(3, i)=p3[(i+3)%4];
  }
  return m.Determinant();
}

#endif
//
