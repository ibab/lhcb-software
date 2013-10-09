#ifndef LORENTZ_MATRIX_HH
#define LORENTZ_MATRIX_HH


#include "TLorentzVector.h"

class LorentzMatrix{
 protected:
  TLorentzVector _v[4];
  // we'll follow the x, y, z, E convention, i.e. E is 4
  bool makeZero(){
    X().SetXYZT(0,0,0,0);
    Y().SetXYZT(0,0,0,0);
    Z().SetXYZT(0,0,0,0);
    T().SetXYZT(0,0,0,0);
    return true;
  }
 public:
  const TLorentzVector& v(int i) const{return _v[i];}

  LorentzMatrix(){};
  LorentzMatrix(const TLorentzVector p[4]){
    for(int i=0; i<4; i++) _v[i] = p[i];
  }
  LorentzMatrix(const LorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other._v[i];
  }
  const TLorentzVector& X() const{return _v[0];}
  const TLorentzVector& Y() const{return _v[1];}
  const TLorentzVector& Z() const{return _v[2];}
  const TLorentzVector& T() const{return _v[3];}

  TLorentzVector& X() {return _v[0];}
  TLorentzVector& Y() {return _v[1];}
  TLorentzVector& Z() {return _v[2];}
  TLorentzVector& T() {return _v[3];}
 
  const TLorentzVector& operator[](int i) const{return _v[i];}
  TLorentzVector& operator[](int i){return _v[i];}

  LorentzMatrix& add(const LorentzMatrix& other){
    for(int i=0; i < 4; i++) _v[i] += other._v[i];
    return *this;
  }
  LorentzMatrix& subtract(const LorentzMatrix& other){
    for(int i=0; i < 4; i++) _v[i] -= other._v[i];
    return *this;
  }
  LorentzMatrix& mult(double s){
    for(int i=0; i < 4; i++) _v[i] *= s;
    return *this;
  }
  LorentzMatrix& div(double s){
    for(int i=0; i < 4; i++) _v[i] *= (1./s);
    return *this;
  }

  LorentzMatrix& operator+=(const LorentzMatrix& rhs){
    return add(rhs);
  }
  LorentzMatrix& operator*=(double rhs){
    return mult(rhs);
  }
  LorentzMatrix& operator-=(const LorentzMatrix& rhs){
    return subtract(rhs);
  }
  LorentzMatrix& operator/=(double rhs){
    return div(rhs);
  }
  LorentzMatrix& operator=(const LorentzMatrix& other){
    for(int i=0; i<4; i++) _v[i] = other._v[i];
    return *this;
  }
  LorentzMatrix operator+(const LorentzMatrix& rhs)const{
    LorentzMatrix returnVal(*this);
    returnVal += rhs;
    return returnVal;
  }
  LorentzMatrix operator-(const LorentzMatrix& rhs)const{
    LorentzMatrix returnVal(*this);
    returnVal -= rhs;
    return returnVal;
  }
  LorentzMatrix operator*(double rhs)const{
    LorentzMatrix returnVal(*this);
    returnVal *= rhs;
    return returnVal;
  }
  LorentzMatrix operator/(double rhs)const{
    LorentzMatrix returnVal(*this);
    returnVal /= rhs;
    return returnVal;
  }

};
#endif
//
