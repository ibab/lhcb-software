#ifndef LEVI_CIVITA_HH
#define LEVI_CIVITA_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/LorentzMatrix.h"
#include "Mint/Utils.h"
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

inline double LeviCivita(const TLorentzVector& p0
			 , const TLorentzVector& p1
			 , const LorentzMatrix& M){
  double sum=0;
  for(int a=0; a<4; a++){
    for(int b=0; b<4; b++){
      if(a==b) continue;
      for(int c=0; c<4; c++){
	if(a==c || b==c) continue;
	for(int d=0; d<4; d++){
	  if(a==d || b==d || c==d) continue;
	  sum += MINT::LeviCita(a, b, c, d)
	    * p0[  (a + 3)%4 ] // coping with the stupid, stupid
	    * p1[  (b + 3)%4 ] // stupid, stupid convention in TLorentzVector
	    * M[ (c + 3)%4 ][ (d + 3)%4 ] // that index 0 = p_x, 1=py, 2=pz, 3=Energy
	    ; // arrrrgh. ARRRRRRRGH!!! Rene!!! Why????
	  // (0 + 3)%4 = 3, (1+3)%4 = 0, (2+3)%4 = 1 (3+3)%4 = 2
	}
      }
    }
  }
  return sum;
}


#endif
//
