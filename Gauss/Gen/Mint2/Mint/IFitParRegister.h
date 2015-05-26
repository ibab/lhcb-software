#ifndef I_FIT_PAR_REGISTER_HH
#define I_FIT_PAR_REGISTER_HH


namespace MINT{
  class IFitParRegister : virtual public IFitParDependent{
  public:
    virtual bool changedSinceLastCall() const=0;
    virtual bool registerFitParDependence(const IFitParDependent& fpd)=0;
  };

}

#endif
//

    
