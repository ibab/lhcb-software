//--------------------------------------------------------------------------
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtItgPtrFunction.hh
//
// Description:
//      Class describing a function with one vector of coefficients. (Stolen and 
//      modified from the BaBar IntegrationUtils package - author: Phil Strother).
//
// Modification history:
//
//    Jane Tinslay                March 21, 2001       Module adapted for use in 
//                                                     EvtGen
//
//------------------------------------------------------------------------

#include "EvtGen/EvtItgPtrFunction.hh"

//-------------
// C Headers --
//-------------
extern "C" {
}

//----------------
// Constructors --
//----------------
EvtItgPtrFunction::EvtItgPtrFunction( double (*theFunction)(double, const HepVector &), double lowerRange, double upperRange, const HepVector &coeffs1):
  EvtItgAbsFunction(lowerRange, upperRange),
  _coeffs1(coeffs1),
  _myFunction(theFunction)
{}


//--------------
// Destructor --
//--------------

EvtItgPtrFunction::~EvtItgPtrFunction( )
{}


double
EvtItgPtrFunction::myFunction(double x) const{
  return _myFunction(x, _coeffs1);
}

void
EvtItgPtrFunction::setCoeff(int vect, int which, double value)
{
  if (vect == 1) _coeffs1[which] = value;
}

double
EvtItgPtrFunction::getCoeff(int vect, int which)
{
  if (vect == 1) return _coeffs1[which];
  else {return 0;}
}
