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
      return (double) _fp; // (_lastValue = (double) _fp);}
    }

    // not entirely sure a copy constructor even makes sense.

  public:
    FitParRef(const FitParameter& fipa, IFitParRegister* daddyPtr=0);
    FitParRef(const FitParRef& other, IFitParRegister* newDaddyPtr=0);
	

    virtual bool changedSinceLastCall() const;
    
    virtual void rememberFitParValues() const{
      _lastValue = (double) _fp;
    }
    
    virtual unsigned int size() const{return 1;}
    virtual const FitParRef& operator[](unsigned int ) const{
      return *this;}


    operator double() const{
      return returnFPVal();
    }

    const FitParameter& theFitParameter() const{ return _fp;}
    double lastValue() const{ return _lastValue;}

    void listFitParDependencies(std::ostream& os=std::cout)const;

    virtual ~FitParRef(){}
  }; 
}
#endif
//
