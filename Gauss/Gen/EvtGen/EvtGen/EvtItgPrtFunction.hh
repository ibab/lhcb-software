//--------------------------------------------------------------------------
//
// Environment:
//      This software was developed for the BaBar collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      LBNL
//
//------------------------------------------------------------------------

#ifndef EVTITGPTRFUNCTION_HH
#define EVTITGPTRFUNCTION_HH

//-------------
// C Headers --
//-------------
extern "C" {
}

#include "EvtGen/EvtItgAbsFunction.hh"

/**
 *  Copyright (C) 1998 LBNL
 *  
 *  Generic function where the pointer to the function is available.
 *
 *  The function is taken as type pointer to function returning double and 
 *  taking a double (the abscissa) and a const RWTValVector<double> reference
 *  (the parameter values of the function) as arguments.
 *
 *  @see EvtItgPtrFunctionEvtItgPtrFunction
 *
 *  @version $Id: EvtItgPrtFunction.hh,v 1.1.1.1 2002-04-11 10:48:20 witoldp Exp $ 
 *
 *  @author Phil Strother       Originator
 */

class EvtItgPtrFunction: public EvtItgAbsFunction {

public:

  // Constructors

  EvtItgPtrFunction( double (*theFunction)(double), double lowerRange, double upperRange);
  
  // Destructor
  virtual ~EvtItgPtrFunction( );
  
protected:
  
  // Helper functions

  virtual double myFunction(double x) const;
  
private:
 
  // Data members

  double (*_myFunction)(double x);
  // Note: if your class needs a copy constructor or an assignment operator, 
  //  make one of the following public and implement it.
  EvtItgPtrFunction( const EvtItgPtrFunction& );                // Copy Constructor
  EvtItgPtrFunction& operator= ( const EvtItgPtrFunction& );    // Assignment op
  
};

#endif // EvtITGPTRFUNCTION_HH
