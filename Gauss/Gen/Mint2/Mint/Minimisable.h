#ifndef MINIMISABLE_HH
#define MINIMISABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/IMinimisable.h"
#include "Mint/MinuitParameterSet.h"

namespace MINT{
class Minimisable : virtual public IMinimisable{
  MinuitParameterSet* _pset;
 public:
  Minimisable(MinuitParameterSet* mps=0);
  Minimisable(const Minimisable& other);
  void setPset(MinuitParameterSet* mps);
  MinuitParameterSet* getParSet();


  // you can choose to implement these:
  virtual void beginFit(){}
  virtual void parametersChanged(){}
  virtual void endFit(){}

 // you'll have to implement this:
  double getVal() =0;

  // useful:
  double getNewVal(){
    parametersChanged();
    return getVal();
  }
};
}//namespace MINT

#endif
//
