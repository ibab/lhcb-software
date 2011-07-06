#ifndef FITTERBASE_HH
#define FITTERBASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "MinuitParameterSet.h"
#include "ILoopable.h"

class FitterBase{
 protected:
  MinuitParameterSet* _parSet;
  TMinuit* _myMinuit;
  ILoopable* _Events;
  //  PDF* pdf;
  
  //  virtual double n2LL(); // -2*logL;

};

#endif
//
