#ifndef TLORENTZ_VECTOR_COMPLEX_HH
#define TLORENTZ_VECTOR_COMPLEX_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status:  Tu 6 May 2014 

#include "TLorentzVector.h"
#include <math.h>
#include <iostream>
#include <complex>

using namespace std;

class TLorentzVectorC{
 protected:
  TLorentzVector _v[2];
  // we'll follow the x, y, z, E convention, i.e. E is 4
  bool makeZero(){
    Re().SetXYZT(0,0,0,0);
    Im().SetXYZT(0,0,0,0);
    return true;
  }
 public:
  const TLorentzVector& v(int i) const{return _v[i];}

  TLorentzVectorC(){};
  TLorentzVectorC(const TLorentzVector& re, const TLorentzVector& im){
    _v[0] = re;
    _v[1] = im;
  }
    
  TLorentzVectorC(const TLorentzVector& re){
        _v[0] = re;
        _v[1].SetXYZT(0.,0.,0.,0.);
  }  
    
  TLorentzVectorC(const TLorentzVectorC& other, bool conj = false){
    _v[0] = other._v[0];
    _v[1] = other._v[1];
    if (conj==true) {
          _v[1]*=-1.;
    }  
  }
  const TLorentzVector& Re() const{return _v[0];}
  const TLorentzVector& Im() const{return _v[1];}

  TLorentzVector& Re() {return _v[0];}
  TLorentzVector& Im() {return _v[1];}
    
  void conj(){
      _v[1]=-_v[1];
  }   
    
  complex<double> Dot(const TLorentzVectorC& rhs){
      return complex<double>(_v[0].Dot(rhs._v[0]) - _v[1].Dot(rhs._v[1]),_v[0].Dot(rhs._v[1]) + _v[1].Dot(rhs._v[0]));
  }

  complex<double> Dot(const TLorentzVector& rhs){
      TLorentzVectorC other=TLorentzVectorC(rhs);
      return Dot(other);
  }  
    
  inline TLorentzVectorC & operator = (const TLorentzVectorC & other) {
         _v[0] = other._v[0];
         _v[1] = other._v[1];
        return *this;
  }
    
  inline TLorentzVectorC operator + (const TLorentzVectorC & other) const {
        return TLorentzVectorC(_v[0] + other._v[0], _v[1] + other._v[1]);
  }
    
  inline TLorentzVectorC operator * (Double_t a) const {
        return TLorentzVectorC(a*_v[0], a*_v[1]);
  }
    
  inline Bool_t operator == (const TLorentzVectorC & other) const {
        return (_v[0] == other._v[0] && _v[1] == other._v[1]);  
  }
        
  void Print(){
      cout << "Re = " ; _v[0].Print();
      cout << "Im = " ; _v[1].Print();
  }  
};
#endif
//
