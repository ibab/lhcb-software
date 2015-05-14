#include "Mint/FitParRef.h"
#include "Mint/IFitParRegister.h"

using namespace MINT;

FitParRef::FitParRef(const FitParameter& fipa, IFitParRegister* daddyPtr)
  : _lastValue( (-9999. != ((double) fipa) ? -9999.0 : -9998.0 ))
  , _fp(fipa){
  if(0 != daddyPtr) daddyPtr->registerFitParDependence(this);
}
//
