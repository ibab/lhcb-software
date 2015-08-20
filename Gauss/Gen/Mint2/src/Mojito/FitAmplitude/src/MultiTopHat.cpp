// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/MultiTopHat.h"

#include "Mint/FitAmplitude.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/FitAmplitude.h"
#include "Mint/IntegCalculator.h"

#include <iostream>
#include <complex>

using namespace std;
using namespace MINT;

MultiTopHat::MultiTopHat()
  : FitAmpListBase()
{
}

MultiTopHat::MultiTopHat(const MultiTopHat& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpListBase(other)
{
}

MultiTopHat::MultiTopHat(const FitAmpListBase& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpListBase(other)
{
}
counted_ptr<FitAmpListBase> MultiTopHat::GetCloneSameFitParameters() const{ 
// need to reform these one day...
// ... it all relies on the copy-constructur/=operator in FitAmpitude
// not copying the fit parameters, but just their pointers
// which will need to be reviewed.
//
  bool dbThis=true;
  if(dbThis) cout << "FitAmpSum::GetCloneSameFitParameters()" << endl;
  /* 
     There'll be 'physical' copies of all Amplitudes, but the
     FitParameters remain the same (pointers to the same
     FitParameter Object).  This is useful for the CP-con coding
     as it is now, but perhaps a bit counter-intuitive.  needs to
     be reviewed at some point. This behaviour is defined in the
     copy constructor of the FitAmplitude class.
  */

  /*
  counted_ptr<MultiTopHat> 
    newList(new MultiTopHat((IDalitzEventList*) this->getEventRecord()
				    , _paraFName.c_str(), _minuitParaSet));
  */
  counted_ptr<MultiTopHat> newList(new MultiTopHat(*this));
  newList->deleteAll();

  newList->add(*this);
  if(dbThis) cout << "cloning MultiTopHat " << newList->size() << endl;
  return newList;
}

MultiTopHat& MultiTopHat::operator=(const MultiTopHat& other){
  if(&other == this) return *this;
  (FitAmpListBase)(*this) = (FitAmpListBase) (other);
  return *this;
}
MultiTopHat& MultiTopHat::operator=(const FitAmpListBase& other){
  if(&other == this) return *this;
  (MultiTopHat)(*this) = other;
  return *this;
}

std::complex<double> MultiTopHat::getVal(IDalitzEvent& evt){
  bool dbthis=false;

  complex<double> sum(0.0, 0.0);

  for(unsigned int i=0; i< this->size(); i++){
    if(dbthis){
      cout << "MultiTopHat::getVal()"
	   << "\n     > for " << getAmpPtr(i)->theBareDecay().oneLiner()
	   << "\n     > I get " << getAmpPtr(i)->getVal(evt)
	   << endl;
    }
    sum += this->getAmpPtr(i)->getVal(evt);
  }

  if(dbthis) cout << "MultiTopHat::getVal(evt):"
		  << " returning this: " << sum 
		  << endl;

  return efficiency(evt)*sum;
}

counted_ptr<IIntegrationCalculator> 
MultiTopHat::makeIntegrationCalculator(){
  return (counted_ptr<IIntegrationCalculator>) makeIntegCalculator();
}
counted_ptr<IntegCalculator> 
MultiTopHat::makeIntegCalculator(){
  counted_ptr<IntegCalculator> l(new IntegCalculator);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    l->addAmps( (_fitAmps[i]),  (_fitAmps[i]));
  }

  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    l->append(_fitAmpLists[i]->makeIntegCalculator());
  }

  cout << "MultiTopHat: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  l->setEfficiency(_efficiency);

  cout << "MultiTopHat::makeIntegCalculator(): returning " << endl;
  l->print();
  return l;
}

counted_ptr<FitAmpPairList> 
MultiTopHat::makeFitAmpPairList(){
  counted_ptr<FitAmpPairList> l(new FitAmpPairList);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    l->addAmps( (_fitAmps[i]),  (_fitAmps[i]));
  }

  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    l->append(_fitAmpLists[i]->makeFitAmpPairList());
  }

  cout << "MultiTopHat: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  l->setEfficiency(_efficiency);


  return l;
}

MultiTopHat::~MultiTopHat(){
  //  deleteAll(); (done by base class)
}

MultiTopHat& 
MultiTopHat::operator+=(const MultiTopHat& other){
  add(other);
  return *this;
}
MultiTopHat 
MultiTopHat::operator+(const MultiTopHat& rhs) const{
  MultiTopHat fas(*this);
  fas.add(rhs);
  return fas;
}


MultiTopHat& MultiTopHat::operator*=(double r){
  multiply(r);
  return *this;
}
MultiTopHat& MultiTopHat::operator*=(const complex<double>& z){
  multiply(z);
  return *this;
}
MultiTopHat& MultiTopHat::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}

MultiTopHat MultiTopHat::operator*(double r) const{
  MultiTopHat fas(*this);
  fas.multiply(r);
  return fas;
}
MultiTopHat MultiTopHat::operator*(const complex<double>& z) const{
  MultiTopHat fas(*this);
  fas.multiply(z);
  return fas;
}
MultiTopHat MultiTopHat::operator*(const counted_ptr<IReturnComplex>& irc) const{
  MultiTopHat fas(*this);
  fas.multiply(irc);
  return fas;
}


MultiTopHat operator*(double r, const MultiTopHat& rhs){
  MultiTopHat fas(rhs);
  fas.multiply(r);
  return fas;
}
MultiTopHat operator*(const complex<double>& z, const MultiTopHat& rhs){
  MultiTopHat fas(rhs);
  fas.multiply(z);
  return fas;
}
MultiTopHat operator*(const counted_ptr<IReturnComplex>& irc
		     , const MultiTopHat& rhs){
  MultiTopHat fas(rhs);
  fas.multiply(irc);
  return fas;
}



//
