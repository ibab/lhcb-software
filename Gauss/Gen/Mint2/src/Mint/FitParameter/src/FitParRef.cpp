#include "Mint/FitParRef.h"
#include "Mint/IFitParRegister.h"

using namespace MINT;
using namespace std;

FitParRef::FitParRef(const FitParameter& fipa, IFitParRegister* daddyPtr)
  : _lastValue( (-9999. != ((double) fipa) ? -9999.0 : -9998.0 ))
  , _fp(fipa){
  /*
  cout << "hello from FitParRef::FitParRef with..." << endl;
  cout << "_lastValue = " << _lastValue << endl;
  cout <<  "_fp " << _fp << endl;
  cout << "daddyPtr = " << daddyPtr << endl;
  */
  if(0 != daddyPtr) daddyPtr->registerFitParDependence(*this);
}
//
