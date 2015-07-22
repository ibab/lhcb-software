#ifndef FERMIPS_BW_LINESHAPE_HH
#define FERMIPS_BW_LINESHAPE_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status: 19 March 2015 GMT

#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"
#include "Mint/NamedParameterBase.h"
#include "Mint/ResonancePropertiesList.h"
#include "Mint/FitParDependent.h"
#include "ResonancePropertiesFitRef.h"

using namespace MINT;

class FermiPS_BW  : public BW_BW, virtual public ILineshape{
 public:
  
  FermiPS_BW( const AssociatedDecayTree& tree): 
    BW_BW(tree), _threshold(0)
    //, _lambda("Fermi_lambda", 1, 2.  , 0, 0, 0, 0,  NamedParameterBase::QUIET)
    //, _s0("Fermi_s0", 1, 2. , 0, 0, 0, 0,  NamedParameterBase::QUIET)    
  {
      /*
      _fittableLambdaPtr = new FitParRef(_lambda, this);
      _fittableS0Ptr = new FitParRef(_s0, this);
      std::cout << "threshold = " << _threshold << std::endl;
      std::cout << "lambda = " << _lambda << std::endl;
      std::cout << "s0 = " << _s0 << std::endl;
       */
      std::vector<const AssociatedDecayTreeItem*> adti = _theDecay.finalState(); 
      for (int i=0; i<adti.size(); i++) {
          _threshold += adti[i]->mass();
      }
      std::cout << "FermiPS_threshold = " << _threshold << " for decay " << _theDecay.oneLiner() << std::endl;
  }

  virtual std::string name() const{
    return "FermiPS_BW("+_theDecay.oneLiner() +")";
  }

  virtual ~FermiPS_BW(){}

 protected:
    //MINT::FitParameter _lambda;
    //MINT::FitParameter _s0;
    //MINT::FitParRef* _fittableLambdaPtr;
    //MINT::FitParRef* _fittableS0Ptr;
    double lambda() const{
        return mumsFittableProperties().fermiLambda();
    }
    double s0() const{
        return mumsFittableProperties().fermiS0();
    }
    double _threshold;
    double phaseSpace(double s);
    virtual double GofM(); 
 
};

#endif
//
