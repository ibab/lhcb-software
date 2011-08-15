// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "Mint/FitAmplitude.h"
#include "Mint/NamedDecayTreeList.h"

#include "Mint/IDalitzEventAccess.h"
#include "Mint/DecayTree.h"

using namespace std;
using namespace MINT;

typedef std::pair<std::string, AmpInitialiser> AmpInitPair;
typedef std::vector<AmpInitPair> AmpInitPairList;

class AmpInitPairLessThan{
 public:
  bool operator()(const AmpInitPair& a1, const AmpInitPair& a2){
    DalitzEventPattern p1 = a1.second.getEventPattern();
    DalitzEventPattern p2 = a2.second.getEventPattern();
    
    //    cout << "compare: got event pattern p1 " << p1 << endl;
    if(p1 == p2){
      return a1.first < a2.first;
    }else{
      return p1 < p2;
    }
    /* this sorts the list such that
       the same final states are grouped
       together - good for printing
    */

  }
};


std::string FitAmplitude::longestNameInList(){
  const NamedDecayTreeList* ndl = NamedDecayTreeList::getMe();
  std::string longestName = ndl->getLongestName();
  
  std::string longA = FitComplexPolar::makeAmpName(longestName);
  std::string longP = FitComplexPolar::makePhaseName(longestName);

  return (longA.size() > longP.size() ? longA : longP);
}

void FitAmplitude::AutogenerateFitFile(const std::string& fname, const DalitzEventPattern& compatiblePattern){

  ofstream os(fname.c_str());
  if(! os) return;

  const NamedDecayTreeList* ndl = NamedDecayTreeList::getMe();
  int nameFieldWidth = longestNameInList().size() +3;

  FitParameter::printFormat(os, nameFieldWidth);

  AmpInitPairList printList;
  for(AmpInitMap::const_iterator it= ndl->trees().begin();
      it != ndl->trees().end();
      it++){
    if(compatiblePattern == DalitzEventPattern::NoPattern || 
       compatiblePattern.compatibleWithFinalState(it->second))
    printList.push_back(*it);
  }
  AmpInitPairLessThan printSorter;
  sort(printList.begin(), printList.end(), printSorter);

  for(AmpInitPairList::const_iterator it= printList.begin();
      it != printList.end();
      it++){
    std::string s;
    os << "*" <<endl;

    s = "\"" + FitComplexPolar::makeAmpName(it->first) + "\"";
    s.resize(nameFieldWidth, ' ');
    os << s << "\t" << FitParameter::getInitString() << endl;
    
    s = "\"" + FitComplexPolar::makePhaseName(it->first) + "\"";
    s.resize(nameFieldWidth, ' ');
    os << s << "\t" << FitParameter::getInitString() << endl; 
  }
  os << "*" << endl;
  os.close();
}

FitAmplitude::FitAmplitude(const std::string& yourOwnName
			   , const AmpInitialiser& treeWithOpts
			   , IDalitzEventAccess* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : FastAmplitude(treeWithOpts, events)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, fname, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(yourOwnName + "_Frac", (double) 0, fname
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(yourOwnName)
{
  if(useStringAs == FitAmplitude::PREFIX){
    _name += treeWithOpts.uniqueName();
  }
}
FitAmplitude::FitAmplitude(const std::string& yourOwnName
			   , const AmpInitialiser& treeWithOpts
			   , IDalitzEventList* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : FastAmplitude(treeWithOpts, events)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, fname, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(yourOwnName + "_Frac", (double) 0, fname
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(yourOwnName)
{
  if(useStringAs == FitAmplitude::PREFIX){
    _name += treeWithOpts.uniqueName();
  }  
}
FitAmplitude::FitAmplitude(const std::string& yourOwnName
			   , const AmpInitialiser& treeWithOpts
			   , IDalitzEventAccess* events
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : FastAmplitude(treeWithOpts, events)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, (char*) 0, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(yourOwnName + "_Frac", (double) 0, (char*) 0
		 		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(yourOwnName)
{
  if(useStringAs == FitAmplitude::PREFIX){
    _name += treeWithOpts.uniqueName();
  }    
}

FitAmplitude::FitAmplitude(const std::string& yourOwnName
			   , const AmpInitialiser& treeWithOpts
			   , IDalitzEventList* events
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : FastAmplitude(treeWithOpts, events)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, (char*) 0, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(yourOwnName + "_Frac", (double) 0, (char*) 0
		 		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(yourOwnName)
{
  if(useStringAs == FitAmplitude::PREFIX){
    _name += treeWithOpts.uniqueName();
  }    
}


FitAmplitude::FitAmplitude(const AmpInitialiser& treeWithOpts
			   , IDalitzEventAccess* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(treeWithOpts, events)
  , _FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), fname, pset
				 , FitParameter::HIDE
				 , NamedParameterBase::QUIET
				 ))
  , _fitFraction(treeWithOpts.uniqueName() + "_Frac", (double) 0, fname
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(treeWithOpts.uniqueName())
{
}
FitAmplitude::FitAmplitude(const AmpInitialiser& treeWithOpts
			   , IDalitzEventList* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(treeWithOpts, events)
  ,_FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), fname, pset
			     , FitParameter::HIDE
			     , NamedParameterBase::QUIET
			     ))
  , _fitFraction(treeWithOpts.uniqueName() + "_Frac", (double) 0, fname
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(treeWithOpts.uniqueName())
{
}

FitAmplitude::FitAmplitude(const AmpInitialiser& treeWithOpts
			   , IDalitzEventAccess* events
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(treeWithOpts, events)
  ,_FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), (char*) 0, pset
			     , FitParameter::HIDE
			     , NamedParameterBase::QUIET
			     ))
  , _fitFraction(treeWithOpts.uniqueName() + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(treeWithOpts.uniqueName())
{
}
FitAmplitude::FitAmplitude(const AmpInitialiser& treeWithOpts
			   , IDalitzEventList* events
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(treeWithOpts, events)
  ,_FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), (char*) 0, pset
			     , FitParameter::HIDE
			     , NamedParameterBase::QUIET
			     ))
  , _fitFraction(treeWithOpts.uniqueName() + "_Frac", (double) 0, (char*) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(treeWithOpts.uniqueName())
{
}

FitAmplitude::FitAmplitude(const std::string& StandardisedDecayTreeName
			   , IDalitzEventAccess* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(AmpInitialiser(StandardisedDecayTreeName)
	      , events)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, fname, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(StandardisedDecayTreeName + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(StandardisedDecayTreeName)
{
  
}
FitAmplitude::FitAmplitude(const std::string& StandardisedDecayTreeName
			   , IDalitzEventList* events
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(AmpInitialiser(StandardisedDecayTreeName)
	      , events)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, fname, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(StandardisedDecayTreeName + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(StandardisedDecayTreeName)
{
  
}

FitAmplitude::FitAmplitude(const std::string& StandardisedDecayTreeName
			   , IDalitzEventAccess* events
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(AmpInitialiser(StandardisedDecayTreeName)
	      , events)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, (char*) 0, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(StandardisedDecayTreeName + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(StandardisedDecayTreeName)
{
  
}
FitAmplitude::FitAmplitude(const std::string& StandardisedDecayTreeName
			   , IDalitzEventList* events
			   , MinuitParameterSet* pset
			   )
  : FastAmplitude(AmpInitialiser(StandardisedDecayTreeName)
	      , events)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, (char*) 0, pset
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(StandardisedDecayTreeName + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(StandardisedDecayTreeName)
{
  
}

FitAmplitude::FitAmplitude(const FitAmplitude& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , IReturnComplex()
  , FastAmplitude(other)
  , _FitAmpPhase(other._FitAmpPhase)
  , _fitFraction(other._fitFraction)
  , _preFactors(other._preFactors)
  , _name(other._name)
{
  /* this creates a copy of the amplitude, but 
     it will depend on the SAME fit parameter 
     (it just copies the (smart) pointer).

     Name will not change since FitParameterName doesn't
     change.

     Since all this might not be 100% intuitive, leave it
     private/protected for now. Use the "Get" methods instead.
  */
}

FitAmplitude::FitAmplitude(const FitAmplitude& other
			   , IDalitzEventAccess* newEvents
			   )
  : IEventAccess<IDalitzEvent>()
  , IReturnReal()
  , IReturnComplex()
  , FastAmplitude(other, newEvents)
  , _FitAmpPhase(other._FitAmpPhase)
  , _fitFraction(other._fitFraction)
  , _preFactors(other._preFactors)
  , _name(other._name)
{
  /* this creates a copy of the amplitude, but 
     it will depend on the SAME fit parameter 
     (it just copies the (smart) pointer).

     Name will not change since FitParameterName doesn't
     change.

     Since all this might not be 100% inuitive, leave it
     private/protected for now. Use the "Get" methods instead.
  */
}
FitAmplitude::FitAmplitude(const FitAmplitude& other
			   , IDalitzEventList* newEvents
			   )
  : IEventAccess<IDalitzEvent>()
  , IReturnReal()
  , IReturnComplex()
  , FastAmplitude(other, newEvents)
  , _FitAmpPhase(other._FitAmpPhase)
  , _fitFraction(other._fitFraction)
  , _preFactors(other._preFactors)
  , _name(other._name)
{
  /* this creates a copy of the amplitude, but 
     it will depend on the SAME fit parameter 
     (it just copies the (smart) pointer).

     Name will not change since FitParameterName doesn't
     change.

     Since all this might not be 100% inuitive, leave it
     private/protected for now. Use the "Get" methods instead.
  */
}

FitAmplitude::~FitAmplitude(){

}

/* now inline
bool FitAmplitude::isZero() const{
  if(0 == _FitAmpPhase) return true;
  return _FitAmpPhase->isZero();
}
*/

bool FitAmplitude::isConstant() const{
  if(0 == _FitAmpPhase) return false;
  return _FitAmpPhase->isConstant();
}
bool FitAmplitude::canBeIgnored() const{
  return isZero() && isConstant();
}
bool FitAmplitude::CPConjugateSameFitParameters(){
  _name += "_CPcon";
  return FastAmplitude::CPConjugate();
}

FitAmplitude FitAmplitude::GetCPConjugateSameFitParameters() const{
  FitAmplitude cp(*this);
  cp.CPConjugateSameFitParameters();
  return cp;
}
/* now inline
FitComplex& FitAmplitude::FitAmpPhase(){
  if(0 == _FitAmpPhase){
    cout << "FATAL ERROR in FitAmplitude::FitAmpPhase()"
	 << "\n    >  _FitAmpPhase pointer zero"
	 << endl;
    throw "should always be set to sth in constructor";
  }
  return *_FitAmpPhase;
}

const FitComplex& FitAmplitude::FitAmpPhase() const{
  if(0 == _FitAmpPhase){
    cout << "FATAL ERROR in FitAmplitude::FitAmpPhase()"
	 << "\n    >  _FitAmpPhase pointer zero"
	 << endl;
    throw "should always be set to sth in constructor";
  }
  return *_FitAmpPhase;
}
*/

/* now inlined:
std::complex<double> FitAmplitude::preFactors(){
  return _preFactors.ComplexVal();
}
*/

/*
  Now inlined - does save a lot of time
  by avoiding the complex constructor
std::complex<double> FitAmplitude::AmpPhase(){
  std::complex<double> FAS(FitAmpPhase());
  if(0.0==FAS) return 0;
  // this will save time in case preFactors() calls
  // some lengthy function and FitAmpPhase is zero
  // anyway.
  return FAS * preFactors();
}
*/


std::complex<double> FitAmplitude::getVal(IDalitzEvent* evt){
  //bool dbThis=false;
  this->setEvent(evt);
  std::complex<double> result(this->getVal());
  this->resetEventRecord();
  return result;
}


std::complex<double> FitAmplitude::getVal(){
  //bool dbThis=false;
  if(isZero()) return 0;
  complex<double> ap(AmpPhase());
  if(0.0 == ap) return 0;


  /*
  if(dbThis){
    std::complex<double> valA = getValWithoutFitParameters();
    if(0 != getEvent()){
      cout << "event pz " << getEvent()->p(1).Z() << endl;
    }
    cout << " FitAmplitude::getVal() for decay " 
	 << theDecay().oneLiner()
	 << "  total = " << AmpPhase() * valA
	 << "\n    >  M()   = " << FitAmpPhase().getVal()
	 << "\n    >  rB*exp()   = " << preFactors()
	 << "\n    >  Amp   = " << valA
	 << endl;
  }
  */
  return  ap * getValWithoutFitParameters();
}


std::complex<double> FitAmplitude::getSmootherLargerVal(IDalitzEvent* evt){
  //bool dbThis=false;
  this->setEvent(evt);
  std::complex<double> result = this->getSmootherLargerVal();
  this->resetEventRecord();
  return result;
}

std::complex<double> FitAmplitude::getSmootherLargerVal(){
  //bool dbThis=false;

  //  if(FitAmpPhase().getAmp() == 0.0) return 0;
  if(AmpPhase() == 0.0) return 0;
  std::complex<double> valA = getSmootherLargerValWithoutFitParameters();

  /*
  if(dbThis){
    cout << " FitAmplitude::getVal() for decay " 
	 << theDecay().oneLiner()
	 << "  total = " << AmpPhase() * valA
	 << "\n    >  M()   = " << FitAmpPhase().getVal()
	 << "\n    >  rB*exp()   = " << preFactors()
	 << "\n    >  Amp   = " << valA
	 << endl;
  }
  */
  return  AmpPhase() * valA;
}
std::complex<double> FitAmplitude::getSmootherLargerValWithoutFitParameters(){
  return FastAmplitude::getSmootherLargerVal();
}

std::complex<double>  FitAmplitude::getValAtResonance(){
  //  cout << "using FitAmplitude::getValAtResonance()" << endl;
  return AmpPhase() * Amplitude::getValAtResonance();
}

/*
double FitAmplitude::gaussProb(){
  double M2 = AmpPhase().getAmp()*AmpPhase().getAmp();

  return M2 * Amplitude::gaussProb();
}
*/

void FitAmplitude::multiply(double r){ // by value
  _preFactors.addTerm(r);
}
void FitAmplitude::multiply(const std::complex<double>& z){ // by value
  _preFactors.addTerm(z);
}
void FitAmplitude::multiply(const counted_ptr<IReturnComplex>& irc){ // by ref
  _preFactors.addTerm(irc);
}

void FitAmplitude::print(std::ostream& os) const{
  //  os << "**FitAmlitude: " << name()
  os << _fitFraction << "\n"
     << FitAmpPhase();
}

FitAmplitude& FitAmplitude::operator*=(double r){
  multiply(r);
  return *this;
}
FitAmplitude& FitAmplitude::operator*=(const complex<double>& z){
  multiply(z);
  return *this;
}
FitAmplitude& FitAmplitude::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}

std::ostream& operator<<(std::ostream& os, const FitAmplitude& fa){
  fa.print(os);
  return os;
}

//
