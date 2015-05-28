#ifndef I_FIT_PAR_DEPENDENT_HH
#define I_FIT_PAR_DEPENDENT_HH

#include <vector>
#include <iostream>

namespace MINT{
  class FitParRef;

  class IFitParDependent{
  public:
    virtual bool changedSinceLastCall() const=0;
    virtual void rememberFitParValues()=0;
    virtual const FitParRef& operator[](unsigned int i) const=0;
    virtual unsigned int size() const=0;

    virtual void listFitParDependencies(std::ostream& os) const=0;
    // (for debugging)
    
  };

}

#endif
//

    
