#ifndef FIT_PAR_REF_HH
#define FIT_PAR_REF_HH

#include "Mint/IFitParDependent.h"
#include "Mint/FitParameter.h"

namespace MINT{
  class IFitParRegister;

  class FitParRef : virtual public IFitParDependent{
    
    mutable double _lastValue;
    
    const FitParameter& _fp;
    double returnFPVal() const{
      return (_lastValue = (double) _fp);}

  public:
    FitParRef(const FitParameter& fipa, IFitParRegister* daddyPtr=0);
    
    // not entirely sure a copy constructor even makes sense.
    FitParRef(const FitParRef& other)
      : _lastValue(other._lastValue), _fp(other._fp){}

    virtual bool changedSinceLastCall() const{
      return (double) _fp != _lastValue;
    }
    
    operator double() const{
      return returnFPVal();
    }
  }; 
}
#endif
//
