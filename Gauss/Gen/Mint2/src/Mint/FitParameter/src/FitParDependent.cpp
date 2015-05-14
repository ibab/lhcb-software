
#include "Mint/FitParDependent.h"

using namespace MINT;

bool FitParDependent::changedSinceLastCall() const{

  for(unsigned int i=0; i < this->size(); i++){
    if( ((*this)[i])->changedSinceLastCall() ) return true;
  }
  return false;
}

FitParDependent::FitParDependent(IFitParRegister* daddy){
  if(0 != daddy) daddy->registerFitParDependence(this);
}

FitParDependent::FitParDependent(const FitParDependent& other)
  : std::vector<const IFitParDependent*>(other){}

