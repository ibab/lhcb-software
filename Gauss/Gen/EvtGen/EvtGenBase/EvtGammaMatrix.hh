//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtGammaMatrix.hh
//
// Description: Class to manipulate gamma matrices. The reperesentation
//              used is the "standard" Dirac representation.
//
// Modification history:
//
//    DJL/RYD     September 25, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTGAMMAMATRIX_HH
#define EVTGAMMAMATRIX_HH

#include "EvtGenBase/EvtComplex.hh"
//#include <iostream>
#include <iosfwd>
class EvtDiracSpinor;
class EvtGammaMatrix;

EvtGammaMatrix operator*(const EvtComplex& c,const EvtGammaMatrix& g);
EvtDiracSpinor operator*(const EvtGammaMatrix& g,const EvtDiracSpinor& d);
EvtComplex operator*(const EvtDiracSpinor& d,const EvtDiracSpinor& dp);
inline EvtGammaMatrix operator+(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
inline EvtGammaMatrix operator-(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
inline EvtGammaMatrix operator*(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
std::ostream& operator<<(std::ostream& s, const EvtGammaMatrix& v);  

//=====================
class EvtGammaMatrix {

  friend EvtGammaMatrix operator*(const EvtComplex& c,const EvtGammaMatrix& g);
  friend EvtDiracSpinor operator*(const EvtGammaMatrix& g,const EvtDiracSpinor& d);
  friend EvtComplex operator*(const EvtDiracSpinor& d,const EvtDiracSpinor& dp);
  friend EvtGammaMatrix operator+(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
  friend EvtGammaMatrix operator-(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
  friend EvtGammaMatrix operator*(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2);
  friend std::ostream& operator<<(std::ostream& s, const EvtGammaMatrix& v);  

public:

  EvtGammaMatrix();
  virtual ~EvtGammaMatrix();
  EvtGammaMatrix(const EvtGammaMatrix& gm);  
  EvtGammaMatrix& operator=(const EvtGammaMatrix& gm);  

  void init();
  //void dump();   
  static const EvtGammaMatrix& g0();
  static const EvtGammaMatrix& g1();
  static const EvtGammaMatrix& g2();
  static const EvtGammaMatrix& g3();
  static const EvtGammaMatrix& g5();
  static const EvtGammaMatrix& id();
  static const EvtGammaMatrix& va0();
  static const EvtGammaMatrix& va1();
  static const EvtGammaMatrix& va2();
  static const EvtGammaMatrix& va3();
  static const EvtGammaMatrix& v0();
  static const EvtGammaMatrix& v1();
  static const EvtGammaMatrix& v2();
  static const EvtGammaMatrix& v3();
  EvtGammaMatrix& operator+=(const EvtGammaMatrix &g);
  EvtGammaMatrix& operator-=(const EvtGammaMatrix &g);
  EvtGammaMatrix& operator*=(const EvtGammaMatrix &g);
  
private:

  EvtComplex gamma[4][4];

};


inline EvtGammaMatrix operator+(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2){
  return EvtGammaMatrix(g1)+=g2;
}

inline EvtGammaMatrix operator-(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2){
  return EvtGammaMatrix(g1)-=g2;
}

inline EvtGammaMatrix operator*(const EvtGammaMatrix& g1,const EvtGammaMatrix& g2){
  return EvtGammaMatrix(g1)*=g2;
}


#endif

