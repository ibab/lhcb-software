
#include "Mint/FitParDependent.h"
#include "Mint/FitParRef.h"
#include <iostream>

using namespace MINT;
using namespace std;

/*
bool FitParDependent::changedSinceLastCall() const{
  //  return true; // for debugging only (effectively switches off caching)
  for(unsigned int i=0; i < this->size(); i++){
    if( ((*this)[i]).changedSinceLastCall() ) return true;
  }
  // cout << "no change since last call " << endl;
  return false;
}
void FitParDependent::rememberFitParValues() const{
  for(unsigned int i=0; i < this->size(); i++){
    ((*this)[i]).rememberFitParValues();
  }
}


const FitParRef& FitParDependent::operator[](unsigned int i) const{
  return ((static_cast<const vector<FitParRef>& >(*this))[i]);
}

*/

bool FitParDependent::ignoreFitParRef(const FitParRef& fpr)const{
  return (0 != fpr.theFitParameter().iFixInit())
    || (0 == fpr.theFitParameter().stepInit());
}

bool FitParDependent::registerFitParDependence(const IFitParDependent& fpd){
  for(unsigned int i=0; i < fpd.size(); i++){
      registerFitParDependence(fpd[i]);
  }
  return true;
}
bool FitParDependent::registerFitParDependence(const FitParRef& fpr){
  if(ignoreFitParRef(fpr)) return false;

  this->push_back(fpr);
  if(0 != _daddy) _daddy->registerFitParDependence(fpr);
  return true;
}
    

FitParDependent::FitParDependent(IFitParRegister* daddy) : _daddy(daddy){}
//  if(0 != daddy) daddy->registerFitParDependence(*this);
// the new scheme is saver as it ensures that everyone has their own
// copy of the FitParRefs, with independent instances of _lastValue.


FitParDependent::FitParDependent(const FitParDependent& other, IFitParRegister* newDaddy)
  : std::vector<FitParRef>(other){
  if(0 != newDaddy) _daddy = newDaddy;
}

void FitParDependent::listFitParDependencies(ostream& os) const{
  for(unsigned int i=0; i < this->size(); i++){
    const FitParameter& fp = ((*this)[i]).theFitParameter();
    double prev = ((*this)[i]).lastValue();
    bool ch = (((double) fp) != prev);
    os << fp << ", prev: " << prev << (ch ? " changed" : "") << ", " 
       << ((*this)[i]).changedSinceLastCall()<< ", "
       << changedSinceLastCall() << endl;
  }
}

//
