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

FitParRef::FitParRef(const FitParRef& other, IFitParRegister* newDaddyPtr)
 : _lastValue( (-9999. != ((double) other) ? -9999.0 : -9998.0 ))
//  : _lastValue(other._lastValue)
  , _fp(other._fp){
  /*
  cout << "hello from FitParRef::FitParRef with..." << endl;
  cout << "_lastValue = " << _lastValue << endl;
  cout <<  "_fp " << _fp << endl;
  cout << "daddyPtr = " << daddyPtr << endl;
  */
  //cout << "FitParRef copy constructor called" << endl;
  if(0 != newDaddyPtr) newDaddyPtr->registerFitParDependence(*this);
}

bool FitParRef::changedSinceLastCall() const{
  /*
  bool changed = ((double) _fp) != _lastValue;
  if(changed) cout << "canged because " << _fp << " != " << _lastValue << endl;
  else cout << "not changed" << endl;
  return changed;
  */
  return ( ((double) _fp) != _lastValue);
}
    
void FitParRef::listFitParDependencies(ostream& os) const{
  bool ch = changedSinceLastCall();
  os << _fp << ", prev: " << _lastValue << (ch ? "changed" : "") << endl;
}

//
