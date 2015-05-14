#ifndef SBW_LINESHAPE_HH
#define SBW_LINESHAPE_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status: 19 March 2015 GMT

#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"
#include "Mint/FitParDependent.h"

class SBW : public BW_BW, virtual public ILineshape{
 public:
  
  SBW( const AssociatedDecayTree& tree): BW_BW(tree){}

  virtual std::string name() const{
    return "SBW("+_theDecay.oneLiner() +")";
  }

  virtual ~SBW(){}

 protected:

  virtual double GofM() {return mumsWidth();}
 
};

#endif
//
