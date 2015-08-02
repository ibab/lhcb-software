#ifndef ICOMPLEX_FITPARDEPENDENT_HH
#define ICOMPLEX_FITPARDEPENDENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <complex>
#include "MINT/IReturnComplex.h"
#include "MINT/IFitParDependent.h"

namespace MINT{
  class IComplexFitParDependent: virtual public IReturnComplex, virtual public IFitParDependent{
  protected:
  };
  
}//namespace MINT
#endif
