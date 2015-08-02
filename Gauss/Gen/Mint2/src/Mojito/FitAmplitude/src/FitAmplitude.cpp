// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "Mint/FitAmplitude.h"
#include "Mint/NamedDecayTreeList.h"

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
			   , const char* fname
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : _amp(treeWithOpts, this)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, fname, pset, this
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
  cout << "pset pointer in FitAmplitude " << pset << endl;
}

FitAmplitude::FitAmplitude(const std::string& yourOwnName
			   , const AmpInitialiser& treeWithOpts
			   , MinuitParameterSet* pset
			   , STRING_USAGE useStringAs
			   )
  : _amp(treeWithOpts, this)
  , _FitAmpPhase(FitComplexMaker(yourOwnName, (char*) 0, pset, this
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
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : _amp(treeWithOpts, this)
  , _FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), fname, pset, this
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
			   , MinuitParameterSet* pset
			   )
  : _amp(treeWithOpts, this)
  ,_FitAmpPhase(FitComplexMaker(treeWithOpts.uniqueName(), (char*) 0, pset, this
			     , FitParameter::HIDE
			     , NamedParameterBase::QUIET
			     ))
  , _fitFraction(treeWithOpts.uniqueName() + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(treeWithOpts.uniqueName())
{
}

FitAmplitude::FitAmplitude(const std::string& StandardisedDecayTreeName
			   , const char* fname
			   , MinuitParameterSet* pset
			   )
  : _amp(AmpInitialiser(StandardisedDecayTreeName), this)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, fname, pset, this
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
			   , MinuitParameterSet* pset
			   )
  : _amp(AmpInitialiser(StandardisedDecayTreeName), this)
  , _FitAmpPhase(FitComplexMaker(StandardisedDecayTreeName, (char*) 0, pset, this
			      , FitParameter::HIDE
			      , NamedParameterBase::QUIET
			      ))
  , _fitFraction(StandardisedDecayTreeName + "_Frac", (double) 0
		 , NamedParameterBase::QUIET)
  , _preFactors(1)
  , _name(StandardisedDecayTreeName)
{
  
}
FitAmplitude::FitAmplitude(const FitAmplitude& other, IFitParRegister* newDaddy)
  : FitParDependent(other, newDaddy)
  , IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , _amp(other._amp, this)
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
  return amp().CPConjugate();
}

FitAmplitude FitAmplitude::GetCPConjugateSameFitParameters() const{
  FitAmplitude cp(*this);
  cp.CPConjugateSameFitParameters();
  return cp;
}

std::complex<double> FitAmplitude::getVal(IDalitzEvent* evt){
  if(0 == evt) return 0;
  return getVal(*evt);
}

std::complex<double> FitAmplitude::getVal(IDalitzEvent& evt){
  bool dbThis=false;
  if(isZero()) return 0;
  complex<double> ap(AmpPhase());
  if(0.0 == ap) return 0;
  if(dbThis){
    cout << "FitAmplitude::getVal(evt) with name:\n"
	 << this->name()
	 << "\n and evt = \n"
	 << evt
	 << "\n returning " << ap << " * "
	 << getValWithoutFitParameters(evt)
	 << "\n ----------------- "
	 << endl;
  }
  return  ap * getValWithoutFitParameters(evt);
}

void FitAmplitude::multiply(double r){ // by value
  _preFactors.addTerm(r);
}
void FitAmplitude::multiply(const std::complex<double>& z){ // by value
  _preFactors.addTerm(z);
}
void FitAmplitude::multiply(const counted_ptr<IComplexFitParDependent>& irc){ // by ref
  _preFactors.addTerm(irc);
}
void FitAmplitude::multiply(const counted_ptr<IReturnComplex>& irc){ // by ref
  _preFactors.addTerm(irc);
}
void FitAmplitude::multiply(const counted_ptr<IComplexForEventFitParDependent<IDalitzEvent> >& irce){ // by ref
  _evt_dep_preFactors.addTerm(irce);
}
void FitAmplitude::multiply(const counted_ptr<IReturnComplexForEvent<IDalitzEvent> >& irce){ // by ref
  _evt_dep_preFactors.addTerm(irce);
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
FitAmplitude& FitAmplitude::operator*=(const counted_ptr<IComplexFitParDependent>& irc){
  multiply(irc);
  return *this;
}
FitAmplitude& FitAmplitude::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}
FitAmplitude& FitAmplitude::operator*=(const counted_ptr<IComplexForEventFitParDependent<IDalitzEvent> >& irce){
  multiply(irce);
  return *this;
}
FitAmplitude& FitAmplitude::operator*=(const counted_ptr<IReturnComplexForEvent<IDalitzEvent> >& irce){
  multiply(irce);
  return *this;
}

std::ostream& operator<<(std::ostream& os, const FitAmplitude& fa){
  fa.print(os);
  return os;
}

//
