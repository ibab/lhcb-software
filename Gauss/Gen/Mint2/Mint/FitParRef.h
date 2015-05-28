#ifndef FIT_PAR_REF_HH
#define FIT_PAR_REF_HH

#include "Mint/IFitParDependent.h"
#include "Mint/IFitParRegister.h"
#include "Mint/FitParameter.h"
#include "FitParameter.h"

namespace MINT{
  class IFitParDependent;
  
  class FitParRef : virtual public IFitParDependent{
    
    double _lastValue;
    
    const FitParameter* _fp;
    const FitParameter& fp() const{return *_fp;}
    double returnFPVal() const{
      return (double) fp(); // (_lastValue = (double) fp());}
    }

    // not entirely sure a copy constructor even makes sense.

  public:
    FitParRef(const FitParameter& fipa, IFitParRegister* daddyPtr=0);
    FitParRef(const FitParRef& other, IFitParRegister* newDaddyPtr=0);
	

    virtual bool changedSinceLastCall() const{
      return ( ((double) fp()) != _lastValue);
    }

    virtual void rememberFitParValues(){
      _lastValue = (double) fp();
    }
    
    virtual unsigned int size() const{return 1;}
    virtual const FitParRef& operator[](unsigned int ) const{
      return *this;}

    operator double() const{
      return returnFPVal();
    }

    const FitParameter& theFitParameter() const{ return fp();}
    double lastValue() const{ return _lastValue;}

    virtual void listFitParDependencies(std::ostream& os=std::cout)const;

    FitParRef& operator=(const FitParRef& rhs);

    ~FitParRef(){}
  }; 
}
#endif
//
