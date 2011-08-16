#include "Mint/SymmLorentzMatrix.h"

#include <iostream>
#include "TLorentzVector.h"

using namespace std;

SymmLorentzMatrix* SymmLorentzMatrix::__gmunu = 0;

const SymmLorentzMatrix& SymmLorentzMatrix::gmunu(){
  if(0 == __gmunu) makeGmunu();
  return * __gmunu;
}


void SymmLorentzMatrix::makeGmunu(){
  __gmunu = new SymmLorentzMatrix();
  __gmunu->X().SetXYZT(-1,0,0,0);
  __gmunu->Y().SetXYZT(0,-1,0,0);
  __gmunu->Z().SetXYZT(0,0,-1,0);
  __gmunu->T().SetXYZT(0,0,0, 1);
}

/*
void SymmLorentzMatrix::makeGmunu(){
  __gmunu = new SymmLorentzMatrix();
  __gmunu->X().SetXYZT(1,0,0,0);
  __gmunu->Y().SetXYZT(0,1,0,0);
  __gmunu->Z().SetXYZT(0,0,1,0);
  __gmunu->T().SetXYZT(0,0,0,1);
}
*/

SymmLorentzMatrix operator*(double lhs, const SymmLorentzMatrix& rhs){
  SymmLorentzMatrix returnVal(rhs);
  returnVal *= lhs;
  return returnVal;
}
SymmLorentzMatrix operator/(double lhs, const SymmLorentzMatrix& rhs){
  SymmLorentzMatrix returnVal(rhs);
  returnVal /= lhs;
  return returnVal;
}
