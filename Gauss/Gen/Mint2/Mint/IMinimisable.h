#ifndef IMINIMISABLE_HH
#define IMINIMISABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include <iostream>
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

  // we should make the following three abstract (=0)
  // and move their default definition to Minimisable.

  virtual void Gradient(Double_t* grad){        
        std::cout << "Gradient of pdf is not implemented. Please implement me or set useAnalyticGradient to 0 in your options file. I'll crash now. " << std::endl;
        throw "crash";
  }
  virtual bool useAnalyticGradient() {return false;}
  virtual void setUseAnalyticGradient(bool useAnalyticGradient){};  
  
  virtual ~IMinimisable(){}
};
}//namespace MINT

#endif
//
