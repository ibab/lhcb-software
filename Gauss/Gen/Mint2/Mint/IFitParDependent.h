#ifndef I_FIT_PAR_DEPENDENT_HH
#define I_FIT_PAR_DEPENDENT_HH


namespace MINT{
  class IFitParDependent{
  public:
    virtual bool changedSinceLastCall() const=0;
  };

}

#endif
//

    
