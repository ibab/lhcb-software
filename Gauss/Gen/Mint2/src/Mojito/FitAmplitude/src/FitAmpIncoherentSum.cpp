// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpIncoherentSum.h"

#include "Mint/FitAmplitude.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/FitAmplitude.h"
#include "Mint/IntegCalculator.h"

#include <iostream>
#include <complex>

using namespace std;
using namespace MINT;

std::string FitAmpIncoherentSum::IncPrefix(){
  return "Inco_";
}

FitAmpIncoherentSum::FitAmpIncoherentSum(const DalitzEventPattern& pat
		     , const char* fname
		     , MinuitParameterSet* pset
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, fname, pset, FitAmpIncoherentSum::IncPrefix()+ prefix, opt)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
}

FitAmpIncoherentSum::FitAmpIncoherentSum(const DalitzEventPattern& pat
		     , MinuitParameterSet* pset
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, pset, FitAmpIncoherentSum::IncPrefix() + prefix, opt)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
}
FitAmpIncoherentSum::FitAmpIncoherentSum(const DalitzEventPattern& pat
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, FitAmpIncoherentSum::IncPrefix() + prefix, opt)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
}

FitAmpIncoherentSum::FitAmpIncoherentSum(const FitAmpIncoherentSum& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]);  
}

FitAmpIncoherentSum::FitAmpIncoherentSum(const FitAmpList& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
{
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]);  
}
counted_ptr<FitAmpList> FitAmpIncoherentSum::GetCloneSameFitParameters() const{ 
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
  counted_ptr<FitAmpIncoherentSum> 
    newList(new FitAmpIncoherentSum((IDalitzEventList*) this->getEventRecord()
				    , _paraFName.c_str(), _minuitParaSet));
  */
  counted_ptr<FitAmpIncoherentSum> newList(new FitAmpIncoherentSum(*this));
  newList->deleteAll();

  newList->add(*this);
  if(dbThis) cout << "cloning FitAmpList " << newList->size() << endl;
  return newList;
}

FitAmpIncoherentSum& FitAmpIncoherentSum::operator=(const FitAmpIncoherentSum& other){
  if(&other == this) return *this;
  (FitAmpList)(*this) = (FitAmpList) (other);
  return *this;
}
FitAmpIncoherentSum& FitAmpIncoherentSum::operator=(const FitAmpList& other){
  if(&other == this) return *this;
  (FitAmpIncoherentSum)(*this) = other;
  return *this;
}

double FitAmpIncoherentSum::getVal(IDalitzEvent& evt){
  bool dbthis=false;

  double sum(0);

  if(_fitAmps.empty()){
    createAllAmps(evt.eventPattern()
		  , FitAmpIncoherentSum::IncPrefix());
  }

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(dbthis){
      cout << "FitAmpIncoherentSum::getVal()"
	   << "\n     > for " << (_fitAmps[i])->theBareDecay().oneLiner()
	   << "\n     > I get " << (_fitAmps[i])->getVal(evt)
	   << endl;
    }
    sum += norm((_fitAmps[i])->getVal(evt));
  }
  if(dbthis) cout << "FitAmpIncoherentSum::getVal(evt):"
		  << " returning this: " << sum 
		  << endl;

  if(false && sum > 200){
    cout << "large FitAmpIncoherentSum " << sum
	 << " the largest amplitude is: "
	 << endl;
    printLargestAmp();
  }

  return efficiency(evt)*sum;

}

counted_ptr<IIntegrationCalculator> 
FitAmpIncoherentSum::makeIntegrationCalculator(){
  return (counted_ptr<IIntegrationCalculator>) makeIntegCalculator();
}
counted_ptr<IntegCalculator> 
FitAmpIncoherentSum::makeIntegCalculator(){
  counted_ptr<IntegCalculator> l(new IntegCalculator);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    l->addAmps( (_fitAmps[i]),  (_fitAmps[i]));
  }

  cout << "FitAmpIncoherentSum: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  l->setEfficiency(_efficiency);
  return l;
}

void FitAmpIncoherentSum::print(std::ostream& os) const{
   os << "FitAmpIncoherentSum::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    os << "\n\t" << (_fitAmps[i])->theBareDecay().oneLiner()
       << endl;
  }
}
void FitAmpIncoherentSum::printNonZero(std::ostream& os) const{
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(_fitAmps[i]->isZero()) continue;
    os << "\n\t" << (_fitAmps[i])->theBareDecay().oneLiner()
       << endl;
  }
}


FitAmpIncoherentSum::~FitAmpIncoherentSum(){
  deleteAll();
}

FitAmpIncoherentSum& 
FitAmpIncoherentSum::operator+=(const FitAmpIncoherentSum& other){
  add(other);
  return *this;
}
FitAmpIncoherentSum 
FitAmpIncoherentSum::operator+(const FitAmpIncoherentSum& rhs) const{
  FitAmpIncoherentSum fas(*this);
  fas.add(rhs);
  return fas;
}


FitAmpIncoherentSum& FitAmpIncoherentSum::operator*=(double r){
  multiply(r);
  return *this;
}
FitAmpIncoherentSum& FitAmpIncoherentSum::operator*=(const complex<double>& z){
  multiply(z);
  return *this;
}
FitAmpIncoherentSum& FitAmpIncoherentSum::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}

FitAmpIncoherentSum FitAmpIncoherentSum::operator*(double r) const{
  FitAmpIncoherentSum fas(*this);
  fas.multiply(r);
  return fas;
}
FitAmpIncoherentSum FitAmpIncoherentSum::operator*(const complex<double>& z) const{
  FitAmpIncoherentSum fas(*this);
  fas.multiply(z);
  return fas;
}
FitAmpIncoherentSum FitAmpIncoherentSum::operator*(const counted_ptr<IReturnComplex>& irc) const{
  FitAmpIncoherentSum fas(*this);
  fas.multiply(irc);
  return fas;
}


FitAmpIncoherentSum operator*(double r, const FitAmpIncoherentSum& rhs){
  FitAmpIncoherentSum fas(rhs);
  fas.multiply(r);
  return fas;
}
FitAmpIncoherentSum operator*(const complex<double>& z, const FitAmpIncoherentSum& rhs){
  FitAmpIncoherentSum fas(rhs);
  fas.multiply(z);
  return fas;
}
FitAmpIncoherentSum operator*(const counted_ptr<IReturnComplex>& irc
		     , const FitAmpIncoherentSum& rhs){
  FitAmpIncoherentSum fas(rhs);
  fas.multiply(irc);
  return fas;
}



//
