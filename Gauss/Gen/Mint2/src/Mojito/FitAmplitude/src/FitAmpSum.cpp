// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpSum.h"
#include "Mint/NamedParameter.h"
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
, _useAnalyticGradient("useAnalyticGradient",0)  
{
  /*
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
  */
}

FitAmpSum::FitAmpSum(const DalitzEventPattern& pat
		     , MinuitParameterSet* pset
		     , const std::string& prefix
		     , const std::string& opt
		     )
: FitAmpList(pat, pset, prefix, opt)
, _useAnalyticGradient("useAnalyticGradient",0)  
{

  // cout << "pset pointer in FitAmpSum::FitAmpSum " << pset << " = " << getMPS() << endl;

  /*
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]);  
  */
}
FitAmpSum::FitAmpSum(const DalitzEventPattern& pat
		     , const std::string& prefix
		     , const std::string& opt
		     )
  : FitAmpList(pat, prefix, opt)
  , _useAnalyticGradient("useAnalyticGradient",0)
{
  /*
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,pat);
    this->getVal(eventTest[0]); 
  */
}

FitAmpSum::FitAmpSum(const FitAmpSum& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
  , _useAnalyticGradient("useAnalyticGradient",0)
{
  /*
    //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]); 
  */
}

FitAmpSum::FitAmpSum(const FitAmpList& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , IFastAmplitudeIntegrable()
  , ILookLikeFitAmpSum()
  , FitAmpList(other)
  , _useAnalyticGradient("useAnalyticGradient",0)
{    
  /*
  //Important! Ensures everything is initialised
    DalitzEventList eventTest;
    eventTest.generatePhaseSpaceEvents(1,_pat);
    this->getVal(eventTest[0]); 
  */
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

counted_ptr<FitAmpListBase> FitAmpSum::GetCloneSameFitParameters() const{ 
// need to reform these one day...
// ... it all relies on the copy-constructur/=operator in FitAmpitude
// not copying the fit parameters, but just their pointers
// which will need to be reviewed.
//
  bool dbThis=false;
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
  if(dbThis) cout << "cloning FitAmpSum " << newList->size() << endl;
  return newList;
}

std::complex<double> FitAmpSum::getVal(IDalitzEvent* evt){
  return getVal(*evt);
}

std::complex<double> FitAmpSum::getVal(IDalitzEvent& evt){
  //double dbThis=false;

  std::complex<double> sum(0);
  if(0 == size()) createAllAmps(evt.eventPattern());
  for(unsigned int i=0; i< this->size(); i++){
    if(! this->getAmpPtr(i)->isZero()){
      sum += this->getAmpPtr(i)->getVal(evt);
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

void FitAmpSum::Gradient(IDalitzEvent& evt,Double_t* grad,MinuitParameterSet* mps){
    
    std::complex<double> val = getVal(evt);
    std::complex<double> valConj= conj(val);
        
    for (unsigned int i=0; i<mps->size(); i++) {
        if(mps->getParPtr(i)->hidden())continue;

        string name_i= mps->getParPtr(i)->name();
        if(name_i.find("Inco")!=std::string::npos)continue;

        if(name_i.find("_Re")!=std::string::npos){
            if(mps->getParPtr(i)->iFixInit() && mps->getParPtr(i+1)->iFixInit()){
                i++;
                continue;
            }
            name_i.replace(name_i.find("_Re"),3,"");
            for(unsigned int j=0; j< this->size(); j++){
	      if(A_is_in_B(name_i, this->getAmpPtr(j)->name())){
		std::complex<double> tmp = 2.*valConj* getAmpPtr(j)->getValWithoutFitParameters(evt);
                    grad[i]= tmp.real();
                    grad[i+1]= -tmp.imag();
                    i++;
                    break;
                }
            }
        }
        // Doesn't work. Don't use! 
        /* 
        else if(name_i.find("_Amp")!=std::string::npos){
            name_i.replace(name_i.find("_Amp"),4,"");
            
            for(unsigned int j=0; j<_fitAmps.size(); j++){
                if(A_is_in_B(name_i, _fitAmps[j]->name())){
                    std::complex<double> tmp = (_fitAmps[j])->getValWithoutFitParameters(evt);
                    grad[i]= 2.*(tmp*valConj).real()/std::abs((_fitAmps[j])->AmpPhase());
                    grad[i+1]= -2.*std::arg((_fitAmps[j])->AmpPhase())*(tmp*(valConj-conj((_fitAmps[j])->getVal(evt)))).imag();
                    i++;
                    break;
                }
            }
        }
        */
        else if(mps->getParPtr(i)->iFixInit())continue;
        else {
            std::cout << "FitAmpSum::Gradient() called. Sorry, I don't know how to calculate the derivative with respect to the fit parameter " << mps->getParPtr(i)->name() << " ! Please implement me or set useAnalytic Gradient to 0 in your options file. I'll crash now. " << std::endl;
            throw "crash";
        }
        
    }
    
} 

void FitAmpSum::print(std::ostream& os) const{
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i< this->size(); i++){
    os << "\n\t" << getAmpPtr(i)->theBareDecay().oneLiner()
       << endl;
  }
}
void FitAmpSum::printNonZero(std::ostream& os) const{
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i< this->size(); i++){
    if(getAmpPtr(i)->isZero()) continue;
    os << "\n\t" << getAmpPtr(i)->theBareDecay().oneLiner()
       << endl;
  }

}
void FitAmpSum::printValues(IDalitzEvent& evt, std::ostream& os){
   os << "FitAmpSum::print\n====================";

  for(unsigned int i=0; i< this->size(); i++){
    complex<double> val = getAmpPtr(i)->getVal(evt);
    os << "\n\t" << getAmpPtr(i)->theBareDecay().oneLiner()
       << val;
    if(norm(val) > 1.e10) os << "  HUGE!!!"  << endl;
    os << endl;
  }
}

counted_ptr<IIntegrationCalculator> FitAmpSum::makeIntegrationCalculator(){
  return (counted_ptr<IIntegrationCalculator>) makeIntegCalculator();
}

// duplication betweem making IntegCalculator and FitAmpPairList should be removed one day...
counted_ptr<IntegCalculator> FitAmpSum::makeIntegCalculator(){
  counted_ptr<IntegCalculator> list(new IntegCalculator);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    for(unsigned int j=i; j < _fitAmps.size(); j++){
      if(_fitAmps[j]->canBeIgnored()) continue;
      // all terms within the list
      list->addAmps( (_fitAmps[i]),  (_fitAmps[j]));
    }
    // cross terms between list, and list-of-list
    for(unsigned int k=0; k < _fitAmpLists.size(); k++){
      for(unsigned int l=0; l < _fitAmpLists[k]->size(); l++){
	if(_fitAmpLists[k]->getAmpPtr(l)->canBeIgnored()) continue;
	list->addAmps(_fitAmps[i], _fitAmpLists[k]->getAmpPtr(l));
      }
    }
  }


  // Now it gets complicated. I want to leave all the 
  // terms within a list to whatever that list
  // wants to do. But we don't want to forget cross terms 
  // between lists.

  // cross terms between list, and list-of-list
  for(unsigned int i=1; i < _fitAmpLists.size(); i++){
    for(unsigned int j=0; j < i; j++){
      for(unsigned int ki = 0; ki < _fitAmpLists[i]->size(); ki++){
	if(_fitAmpLists[i]->getAmpPtr(ki)->canBeIgnored()) continue;
	for(unsigned int kj = 0; kj < _fitAmpLists[j]->size(); kj++){
	  if(_fitAmpLists[j]->getAmpPtr(kj)->canBeIgnored()) continue;
	  list->addAmps(_fitAmpLists[i]->getAmpPtr(ki), _fitAmpLists[j]->getAmpPtr(kj));
	}
      }
    }
  }


  // finally, the terms within each list
  // which is up to the list itself.
  // (this is where we save a lot of space and time
  // when it comes to the model-independent stuff
  // where there are no internal cross terms)
  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    list->append(_fitAmpLists[i]->makeIntegCalculator());
  }

  cout << "FitAmpSum: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  list->setEfficiency(_efficiency);

  
  cout << "this->size()" << this->size() << endl;
  cout << "_fitAmpLists.size()" << _fitAmpLists.size() << endl;
  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    cout << "printing list number " << i << endl;
    _fitAmpLists[i]->print();
    cout << endl;
  }
  cout << " the pair list: " << endl;
  list->print();
  
  return list;
}

counted_ptr<FitAmpPairList> FitAmpSum::makeFitAmpPairList(){
  counted_ptr<FitAmpPairList> list(new FitAmpPairList);
  for(unsigned int i=0; i < _fitAmps.size(); i++){
    if(_fitAmps[i]->canBeIgnored()) continue;
    for(unsigned int j=i; j < _fitAmps.size(); j++){
      if(_fitAmps[j]->canBeIgnored()) continue;
      // all terms within the list
      list->addAmps( (_fitAmps[i]),  (_fitAmps[j]));
    }
    // cross terms between list, and list-of-list
    for(unsigned int k=0; k < _fitAmpLists.size(); k++){
      for(unsigned int l=0; l < _fitAmpLists[k]->size(); l++){
	if(_fitAmpLists[k]->getAmpPtr(l)->canBeIgnored()) continue;
	list->addAmps(_fitAmps[i], _fitAmpLists[k]->getAmpPtr(l));
      }
    }
  }


  // Now it gets complicated. I want to leave all the 
  // terms within a list to whatever that list
  // wants to do. But we don't want to forget cross terms 
  // between lists.

  // cross terms between list, and list-of-list
  for(unsigned int i=1; i < _fitAmpLists.size(); i++){
    for(unsigned int j=0; j < i; j++){
      for(unsigned int ki = 0; ki < _fitAmpLists[i]->size(); ki++){
	if(_fitAmpLists[i]->getAmpPtr(ki)->canBeIgnored()) continue;
	for(unsigned int kj = 0; kj < _fitAmpLists[j]->size(); kj++){
	  if(_fitAmpLists[j]->getAmpPtr(kj)->canBeIgnored()) continue;
	  list->addAmps(_fitAmpLists[i]->getAmpPtr(ki), _fitAmpLists[j]->getAmpPtr(kj));
	}
      }
    }
  }


  // finally, the terms within each list
  // which is up to the list itself.
  // (this is where we save a lot of space and time
  // when it comes to the model-independent stuff
  // where there are no internal cross terms)
  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    list->append(_fitAmpLists[i]->makeFitAmpPairList());
  }

  cout << "FitAmpSum: setting efficiency POINTER "
       << " in integCalculator to " 
       << _efficiency.get();
  if(0 == _efficiency.get()){
    cout << " (0 means no pointer, 100% efficiency).";
  }
  cout << endl;

  list->setEfficiency(_efficiency);

  /*
  cout << "this->size()" << this->size() << endl;
  cout << "_fitAmpLists.size()" << _fitAmpLists.size() << endl;
  for(unsigned int i=0; i < _fitAmpLists.size(); i++){
    cout << "printing list number " << i << endl;
    _fitAmpLists[i]->print();
    cout << endl;
  }
  cout << " the pair list: " << endl;
  list->print();
  */

  return list;
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
