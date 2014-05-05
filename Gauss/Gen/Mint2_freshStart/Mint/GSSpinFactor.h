#ifndef GS_SPIN_FACTOR_HH
#define GS_SPIN_FACTOR_HH

#include "Mint/SpinFactor.h"
#include "Mint/SpinFactor3.h"
#include "Mint/DecayTree.h"

class IDalitzEventAccess;

class GSSpinFactor : public SpinFactor3{

 public:
 GSSpinFactor(const DecayTree& theDecay)
   : SpinFactor3(theDecay){}
 
  virtual double getVal(IDalitzEvent& evt);

  virtual std::string name() const{
    return "SpinFactor3:GSSpinFactor("
      + theBareDecay().oneLiner() + ")";
  }
};

#endif
//

