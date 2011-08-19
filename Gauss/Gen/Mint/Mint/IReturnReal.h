#ifndef IRETURN_REALVALUE_HH
#define IRETURN_REALVALUE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

//#include "Mint/IReturnComplex.h"
#include <complex>

namespace MINT{

class IReturnReal{
 protected:
  IReturnReal(){};
 public:
  virtual double RealVal()=0;
  virtual ~IReturnReal(){};
};

}//namespace MINT
#endif
