#ifndef IDALITZ_INTEGRATOR_HH
#define IDALITZ_INTEGRATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/IReturnReal.h"

namespace MINT{
  class Minimiser;
}
class IDalitzIntegrator : virtual public MINT::IReturnReal{
 public:
  virtual void doFinalStats(MINT::Minimiser* mini=0)=0;
};


#endif
//
