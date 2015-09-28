#ifndef RHOOMEGAGS_LINESHAPE_HH
#define RHOOMEGAGS_LINESHAPE_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// Rho-omega mixing as suggested in arXiv:hep-ex/0112031v1

#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/GounarisSakurai.h"
#include "Mint/NamedParameter.h"
#include "Mint/NamedParameterBase.h"
#include "Mint/ResonancePropertiesList.h"
#include "Mint/FitParDependent.h"
#include "ResonancePropertiesFitRef.h"

using namespace MINT;

class RhoOmegaGS  : public GounarisSakurai, virtual public ILineshape{
 public:
  
  RhoOmegaGS( const AssociatedDecayTree& tree): 
    GounarisSakurai(tree),omega_pid(223)   
  {
      setOmegaFitParameters();
  }

  virtual std::string name() const{
    return "RhoOmegaGS("+_theDecay.oneLiner() +")";
  }

  virtual ~RhoOmegaGS(){
      delete _fittableOmegaPropertiesPtr;
  }

 protected:
    bool setOmegaFitParameters();
    ResonancePropertiesFitRef* _fittableOmegaPropertiesPtr;    
    const ResonanceProperties* omegaProperties() const;
    ResonancePropertiesFitRef& omegaFittableProperties() const;
    
    const int omega_pid;
    double omegaMass() const{
        return omegaFittableProperties().mass();
    }
    double omegaWidth() const{
        return omegaFittableProperties().width();
    }

    double delta_Re() const{
        return mumsFittableProperties().rhoOmegaDelta_Re();
    }
    double delta_Im() const{
        return mumsFittableProperties().rhoOmegaDelta_Im();
    }
    std::complex<double> omegaBW(IDalitzEvent& evt);
    virtual std::complex<double> getVal(IDalitzEvent& evt); 
 
};

#endif
//
