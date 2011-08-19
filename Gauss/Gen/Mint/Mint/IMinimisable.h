#ifndef IMINIMISABLE_HH
#define IMINIMISABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/MinuitParameterSet.h"
namespace MINT{
class IMinimisable{
 public:
  virtual MinuitParameterSet* getParSet() = 0;

  virtual void beginFit()=0;
  virtual void parametersChanged()=0;
  virtual void endFit()=0;

  virtual double getVal()=0;
  virtual double getNewVal()=0;
  virtual ~IMinimisable(){}
};
}//namespace MINT

#endif
//
