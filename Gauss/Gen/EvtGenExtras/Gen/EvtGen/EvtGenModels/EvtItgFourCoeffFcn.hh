//--------------------------------------------------------------------------
//
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtItgFourCoeffFcn.hh
//
// Description:
//      Class describing a function with Four vectors of coefficients. 
//
// Modification history:
//
//    Jane Tinslay                March 21, 2001       Module created
//
//------------------------------------------------------------------------

#ifndef EVTITFOURCOEFFFCN_HH
#define EVTITFOURCOEFFFCN_HH

#include "CLHEP/Matrix/Vector.h"
#include "EvtGenModels/EvtItgAbsFunction.hh"

class EvtItgFourCoeffFcn: public EvtItgAbsFunction {

public:

  EvtItgFourCoeffFcn( double (*theFunction)(double, const HepVector &, const HepVector &, const HepVector &, const HepVector &),
		     double lowerRange, double upperRange, const HepVector &coeffs1, const HepVector &coeffs2, const HepVector &coeffs3, const HepVector &coeffs4);

  virtual ~EvtItgFourCoeffFcn( );

  virtual void setCoeff(int, int, double);
  virtual double getCoeff(int, int);

protected:

  virtual double myFunction(double x) const;

private:
 
  // Data members
  double (*_myFunction)(double x, const HepVector & coeffs1, const HepVector & coeffs2, const HepVector & coeffs3, const HepVector & coeffs4);
  
  // Note: if your class needs a copy constructor or an assignment operator, 
  //  make one of the following public and implement it.
  EvtItgFourCoeffFcn( const EvtItgFourCoeffFcn& );                //// Copy Constructor
  EvtItgFourCoeffFcn& operator= ( const EvtItgFourCoeffFcn& );    // Assignment op
  HepVector _coeffs1;
  HepVector _coeffs2;
  HepVector _coeffs3;
  HepVector _coeffs4;

};

#endif // EvtITGPTRFUNCTION_HH
