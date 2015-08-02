// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpSum.h"

#include "Mint/FitAmplitude.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/FitAmplitude.h"
#include "Mint/IntegCalculator.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace MINT;

// note - this inherits now from FitAmpList, many
// methods formerly defined here are now defined there.

FitAmpSum::FitAmpSum(const DalitzEventPattern& pat
		     , const char* fname
		     , MinuitParameterSet* pset
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, fname, pset, prefix, opt)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
}

FitAmpSum::FitAmpSum(const DalitzEventPattern& pat
		     , MinuitParameterSet* pset
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, pset, prefix, opt)
{

  cout << "pset pointer in FitAmpSum::FitAmpSum " << pset << " = " << getMPS() << endl;

    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
}
FitAmpSum::FitAmpSum(const DalitzEventPattern& pat
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, prefix, opt)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]); 
}

FitAmpSum::FitAmpSum(const FitAmpSum& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]); 
}

FitAmpSum::FitAmpSum(const FitAmpList& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
{    
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]); 
}

FitAmpSum& FitAmpSum::operator=(const FitAmpSum& other){
  if(&other == this) return *this;
  (FitAmpList)(*this) = (FitAmpList) (other);
  return *this;
}
FitAmpSum& FitAmpSum::operator=(const FitAmpList& other){
  if(&other == this) return *this;
  (FitAmpSum)(*this) = other;
  return *this;
}

counted_ptr<FitAmpList> FitAmpSum::GetCloneSameFitParameters() const{ 
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
  counted_ptr<FitAmpSum> 
    newList(new FitAmpSum((IDalitzEventList*) this->getEventRecord()
			   , _paraFName.c_str(), _minuitParaSet));
  */
  counted_ptr<FitAmpSum> newList(new FitAmpSum(*this));
  newList->deleteAll();

  if(dbThis) cout << "FitAmpSum::GetCloneSameFitParameters() made ptr" << endl;
  newList->add(*this);
  if(dbThis) cout << "cloning FitAmpList " << newList->size() << endl;
  return newList;
}

std::complex<double> FitAmpSum::getVal(IDalitzEvent* evt){
  return getVal(*evt);
}

std::complex<double> FitAmpSum::getVal(IDalitzEvent& evt){
  //double dbThis=false;

  std::complex<double> sum(0);
  if(_fitAmps.empty()) createAllAmps(evt.eventPattern());
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(! _fitAmps[i]->isZero()){
      sum += (_fitAmps[i])->getVal(evt);
    }
  }
  
  std::complex<double> result(sqrt(fabs(efficiency(evt)))*sum);
  /* 
  double resultSq = norm(result);
  bool invalid = (! isfinite(resultSq)) || std::isnan(resultSq);
  if(dbThis || invalid){
    cout << "\n---------------------------" << endl;
    cout << "sqrt(efficiency())*sum = "
	 << "sqrt(" << efficiency(evt) << ")*" << sum
	 << endl;
    printValues(evt);
    cout << "\n---------------------------" << endl;
  }
   */
  return result;
}

void FitAmpSum::print(std::ostream& os) const{
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    os << "\n\t" << (_fitAmps[i])->theBareDecay().oneLiner()
       << endl;
  }
}
void FitAmpSum::printNonZero(std::ostream& os) const{
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(_fitAmps[i]->isZero()) continue;
    os << "\n\t" << (_fitAmps[i])->theBareDecay().oneLiner()
       << endl;
  }
}
void FitAmpSum::printValues(IDalitzEvent& evt, std::ostream& os){
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    complex<double> val = _fitAmps[i]->getVal(evt);
    os << "\n\t" << (_fitAmps[i])->theBareDecay().oneLiner()
       << val;
    if(norm(val) > 1.e10) os << "  HUGE!!!"  << endl;
    os << endl;
  }
}

counted_ptr<IIntegrationCalculator> FitAmpSum::makeIntegrationCalculator(){
  return (counted_ptr<IIntegrationCalculator>) makeIntegCalculator();
}

counted_ptr<IntegCalculator> FitAmpSum::makeIntegCalculator(){
  counted_ptr<IntegCalculator> l(new IntegCalculator);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    for(unsigned int j=i; j < _fitAmps.size(); j++){
      if(_fitAmps[j]->canBeIgnored()) continue;
      l->addAmps( (_fitAmps[i]),  (_fitAmps[j]));
    }
  }

  cout << "FitAmpSum: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  l->setEfficiency(_efficiency);
  return l;
}

FitAmpSum::~FitAmpSum(){
  deleteAll();
}

FitAmpSum& FitAmpSum::operator+=(const FitAmpSum& other){
  add(other);
  return *this;
}
FitAmpSum FitAmpSum::operator+(const FitAmpSum& rhs) const{
  FitAmpSum fas(*this);
  fas.add(rhs);
  return fas;
}

FitAmpSum& FitAmpSum::operator*=(double r){
  multiply(r);
  return *this;
}
FitAmpSum& FitAmpSum::operator*=(const complex<double>& z){
  multiply(z);
  return *this;
}
FitAmpSum& FitAmpSum::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}
FitAmpSum& FitAmpSum::operator*=(const counted_ptr<IReturnComplexForEvent<IDalitzEvent> >& ircfe){
  multiply(ircfe);
  return *this;
}

FitAmpSum FitAmpSum::operator*(double r) const{
  FitAmpSum fas(*this);
  fas.multiply(r);
  return fas;
}
FitAmpSum FitAmpSum::operator*(const complex<double>& z) const{
  FitAmpSum fas(*this);
  fas.multiply(z);
  return fas;
}
FitAmpSum FitAmpSum::operator*(const counted_ptr<IReturnComplex>& irc) const{
  FitAmpSum fas(*this);
  fas.multiply(irc);
  return fas;
}
FitAmpSum FitAmpSum::operator*(const counted_ptr<IReturnComplexForEvent<IDalitzEvent> >& ircfe) const{
  FitAmpSum fas(*this);
  fas.multiply(ircfe);
  return fas;
}

FitAmpSum operator*(double r, const FitAmpSum& rhs){
  FitAmpSum fas(rhs);
  fas.multiply(r);
  return fas;
}
FitAmpSum operator*(const complex<double>& z, const FitAmpSum& rhs){
  FitAmpSum fas(rhs);
  fas.multiply(z);
  return fas;
}
FitAmpSum operator*(const counted_ptr<IReturnComplex>& irc
		     , const FitAmpSum& rhs){
  FitAmpSum fas(rhs);
  fas.multiply(irc);
  return fas;
}
FitAmpSum operator*(const counted_ptr<IReturnComplexForEvent<IDalitzEvent> >& ircfe
		     , const FitAmpSum& rhs){
  FitAmpSum fas(rhs);
  fas.multiply(ircfe);
  return fas;
}



//
