#ifndef IPDF_HH
#define IPDF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IReturnRealForEvent.h"
#include "Mint/MinuitParameterSet.h"
#include <iostream>

namespace MINT{

template<typename EVENT>
class IPdf : virtual public IReturnRealForEvent<EVENT>{
 public:
  virtual double getVal(EVENT & EVT)=0;
  virtual double RealVal(EVENT & EVT)=0;

  virtual void beginFit()=0;
  virtual void parametersChanged()=0;
  virtual void endFit()=0;

  virtual void Gradient(EVENT & evt, Double_t* grad, MINT::MinuitParameterSet* mps){        
        std::cout << "Gradient of pdf is not implemented. Please implement me or set useAnalyticGradient to 0 in your options file. I'll crash now. " << std::endl;
        throw "crash";
  }
  virtual bool useAnalyticGradient() {return false;}
  //  virtual IPdf<EVENT>* Clone() const=0;

};
}// namespace MINT
#endif
//
