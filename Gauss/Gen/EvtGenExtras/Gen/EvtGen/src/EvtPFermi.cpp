//-----------------------------------------------------------------------
// File and Version Information:
//
// Copyright Information: See EvtGen/COPYRIGHT
//
//
// Description:
//   F(k+) = N (1-x)^a exp((1+a)x) ,x=k+/(mB-mb) 
//   the fermi motion distribution according to
//   hep-ph/9905351 v2
//   
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------
//-----------------------
// This Class's Header --
//-----------------------
#include "EvtGenModels/EvtPFermi.hh"

//---------------
// C Headers --
//---------------
#include <math.h>

//----------------
// Constructors --
//----------------

EvtPFermi::EvtPFermi(const double &a, const double &mB, const double &mb)
{
  _a = a;
  _mb = mb;
  _mB = mB;
}

//--------------
// Destructor --
//--------------

EvtPFermi::~EvtPFermi( )
{
}

//-----------
// Methods --
//-----------

double EvtPFermi::getFPFermi(const double &kplus)
{
  double FKplus;
  double x = kplus/(_mB-_mb);

  if ( x      >= 1)   return 0;
  if ( kplus <= -_mb) return 0; 

  FKplus = pow(1-x,_a)*exp((1+_a)*x);

  return FKplus;
}
