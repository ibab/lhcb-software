#ifndef GS_SPIN_FACTOR_HH
#define GS_SPIN_FACTOR_HH

#include "Mint/Mojito/SpinFactors/SpinFactor.h"
#include "Mint/Mojito/SpinFactors/SpinFactor3.h"
#include "Mint/Mojito/DecayTrees/DecayTree.h"

class IDalitzEventAccess;

class GSSpinFactor : public SpinFactor3{

 public:
 GSSpinFactor(IDalitzEventAccess* events
	      , const DecayTree& theDecay)
   : SpinFactor3(events, theDecay){}
 
  virtual double getVal();

  virtual std::string name() const{
    return "SpinFactor3:GSSpinFactor("
      + theDecay().oneLiner() + ")";
  }
};

#endif
//

