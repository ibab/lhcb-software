// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpList.h"

#include "Mint/IDalitzEventAccess.h"
#include "Mint/FitAmplitude.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpPairList.h"

#include <iostream>

using namespace std;
using namespace MINT;

FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , const char* fname
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(pat)
  , _minuitParaSet(pset)
  , _efficiency(0)
  , _opt(opt)
{
  if(0 != fname){
    _paraFName = fname;
  }else{
    _paraFName.clear();
  }
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(pat)
  , _minuitParaSet(pset)
  , _opt(opt)
{
  
  _paraFName.clear();
  
  createAllAmps(prefix);
}
FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , const std::string& prefix
		       , const std::string& opt
		       )
  : DalitzEventAccess(pat)
  , _minuitParaSet(0)
  , _efficiency(0)
  , _opt(opt)
{
  
  _paraFName.clear();
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(IDalitzEventAccess* events
		       , const char* fname
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(events)
  , _minuitParaSet(pset)
  , _efficiency(0)
  , _opt(opt)
{
  if(0 != fname){
    _paraFName = fname;
  }else{
    _paraFName.clear();
  }
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(IDalitzEventAccess* events
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(events)
  , _minuitParaSet(pset)
  , _efficiency(0)
  , _opt(opt)
{

  _paraFName.clear();
  
  createAllAmps(prefix);
}
FitAmpList::FitAmpList(IDalitzEventAccess* events
		       , const std::string& prefix
		       , const std::string& opt
		       )
  : DalitzEventAccess(events)
  , _minuitParaSet(0)
  , _efficiency(0)
  , _opt(opt)
{

  _paraFName.clear();
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(IDalitzEventList* events
		       , const char* fname
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		       )
  : DalitzEventAccess(events)
  , _minuitParaSet(pset)
  , _opt(opt)
{
  if(0 != fname){
    _paraFName = fname;
  }else{
    _paraFName.clear();
  }
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(IDalitzEventList* events
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(events)
  , _minuitParaSet(pset)
  , _efficiency(0)
  , _opt(opt)
{

  _paraFName.clear();
  
  createAllAmps(prefix);
}
FitAmpList::FitAmpList(IDalitzEventList* events
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : DalitzEventAccess(events)
  , _minuitParaSet(0)
  , _efficiency(0)
  , _opt(opt)
{

  _paraFName.clear();
  
  createAllAmps(prefix);
}

FitAmpList::FitAmpList(const FitAmpList& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , DalitzEventAccess(other)
  , _paraFName(other._paraFName)
  , _minuitParaSet(other._minuitParaSet)
  , _efficiency(other._efficiency)
  , _opt(other._opt)
{
  bool dbThis=false;
  this->deleteAll();
  if(dbThis)cout << "copy-ctor FitAmpList, done deleteAll()" << endl;
  /* 
     There'll be 'physical' copies of all Amplitudes, but the
     FitParameters remain the same (pointers to the same
     FitParameter Object).  This is useful for the CP-con coding
     as it is now, but perhaps a bit counter-intuitive.  Needs to
     be reviewed at some point. This behaviour is defined in the
     copy construcopy-ctor of the FitAmplitude class.
  */

  addCopyWithSameFitParameters(other);
}

FitAmpList& FitAmpList::operator=(const FitAmpList& other){
  if(&other == this) return *this;
  (DalitzEventAccess)(*this) = (DalitzEventAccess) (other);

  _paraFName     = other._paraFName;
  _minuitParaSet = other._minuitParaSet;
  _efficiency    = other._efficiency;
  _opt           = other._opt;
  deleteAll();
  
  addCopyWithSameFitParameters(other);
  return *this;
}
int FitAmpList::add(const FitAmpList& other, double factor){
  return addCopyWithSameFitParameters(other, factor);
}
int FitAmpList::addCopyWithSameFitParameters(const FitAmpList& other
					     , double factor){
  for(unsigned int i=0; i < other._fitAmps.size(); i++){
    FitAmplitude* fa = other._fitAmps[i];
    FitAmplitude* newFa = new FitAmplitude(*fa, (IDalitzEventAccess*) this);
    if(1.0 != factor) newFa->multiply(factor);
    _fitAmps.push_back(newFa);
  }
  return this->size();
}

unsigned int FitAmpList::size() const{
  return _fitAmps.size();
}

FitAmplitude* FitAmpList::getAmpPtr(int i){
  if(i < 0 || i >= (int) _fitAmps.size()){
    cout << " FitAmpList::getAmp index out of range"
	 << endl;
    return 0;
  }
  return _fitAmps[i];
}
const FitAmplitude* FitAmpList::getAmpPtr(int i) const{
  if(i < 0 || i >= (int) _fitAmps.size()){
    cout << " FitAmpList::getAmp index out of range"
	 << endl;
    return 0;
  }    
  return _fitAmps[i];
}


bool FitAmpList::CPConjugateSameFitParameters(){
  bool dbThis=false;
  if(dbThis) cout << "FitAmpList::CPConjugateSameFitParameters()" << endl;

  bool success=true;
  for(unsigned int i=0; i< _fitAmps.size(); i++){
    success &= (_fitAmps[i])->CPConjugateSameFitParameters();
  }
  return success;
}

counted_ptr<FitAmpList> FitAmpList::GetCloneSameFitParameters() const{ 
  bool dbThis=false;
  if(dbThis) cout << "FitAmpList::GetCloneSameFitParameters()" << endl;
  /* 
     There'll be 'physical' copies of all Amplitudes, but the
     FitParameters remain the same (pointers to the same
     FitParameter Object).  This is useful for the CP-con coding
     as it is now, but perhaps a bit counter-intuitive.  needs to
     be reviewed at some point. This behaviour is defined in the
     copy constructor of the FitAmplitude class.
  */

  counted_ptr<FitAmpList> newList(new FitAmpList(*this));
  return newList;
}

counted_ptr<FitAmpList> FitAmpList::GetCPConjugateSameFitParameters() const{
  bool dbThis=false;
  if(dbThis) cout << "FitAmpList::GetCPConjugateSameFitParameters()" << endl;

  counted_ptr<FitAmpList> newList = this->GetCloneSameFitParameters();
  newList->CPConjugateSameFitParameters();
  return newList;
}

bool FitAmpList::createAllAmps(const std::string& prefix){
  if(0 == getEvent()){
    cout << "NOTE in FitAmpList::createAllAmps()"
	 << " I got called, but there is no event."
	 << "\n\t> Need at least one event to determine final state."
	 << "\n\t> Alternatively call "
	 << " createAllAmps(const DalitzEventPattern& thePattern)"
	 << endl;
    return false;
  }

  DalitzEventPattern thePattern = getEvent()->eventPattern();
  // this assumes that the final state is the same
  // for all events in list. No point, really, in
  // adding amplitudes for different final states.

  return createAllAmps(thePattern, prefix);
}
bool FitAmpList::createAllAmps(const DalitzEventPattern& thePattern
			      , const std::string& prefix){
  bool dbThis=false;

  const NamedDecayTreeList* ndl = NamedDecayTreeList::getMe();

  if(! ndl){
    cout << "ERROR in FitAmpList::createAllAmps!"
	 << "\n   > Can't get NamedDecayTreeList."
	 << endl;
    return false;
  }
 
  if(ndl->size() == 0){
    cout << "ERROR in FitAmpList::createAllAmps!"
	 << "\n   > NamedDecayTreeList is empty."
	 << endl;
    return false;
  }

  bool success=true;
  const char* fnamePtr = 0;
  if( ! _paraFName.empty()){
    fnamePtr = _paraFName.c_str();
  }
  for(AmpInitMap::const_iterator it= ndl->trees(_opt).begin();
      it != ndl->trees(_opt).end();
      it++){

    const DecayTree& theTree = it->second.tree();
    if(! thePattern.compatibleWith(theTree)) continue;
    if(dbThis) cout << "making amplitude " << prefix + it->first << endl;
    FitAmplitude* fa = 
      new FitAmplitude(prefix + it->first
		       , it->second
		       , this
		       , fnamePtr
		       , _minuitParaSet
		      );
    if(0 == fa){
      cout << "ERROR in FitAmpList::createAllAmps!"
	   << "\n   > failed to create FitAmplitude"
	   << "\n   > for decay name " << it->first
	   << "\n   > \"new\" returned zero."
	   << "\n   > I'll keep going and try to"
	   << "\n   > create the other amplitudes."
	   << endl;
      success = false;
      continue;
    }
    if(dbThis) cout << "check init values: " << *fa << endl;
    _fitAmps.push_back(fa);
  }
  return success;
}

void FitAmpList::printLargestAmp(std::ostream& os){
  bool dbthis=false;
  if(_fitAmps.empty()) createAllAmps();
  double largestValue = -9999;
  std::string largestName = "none";

  if(0 == getEvent()){
    os << "FitAmpList::printLargestAmp(): no Event, no Amp"
       << endl;
    return;
  }

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(dbthis){
      cout << "FitAmpList::printLargestAmp()"
	   << "\n     > for " << (_fitAmps[i])->theDecay().oneLiner()
	   << "\n     > I get " << (_fitAmps[i])->getVal()
	   << endl;
    }
    double val = norm((_fitAmps[i])->getVal());
    if(val > largestValue){
      largestValue = val;
      largestName = (_fitAmps[i])->name();
    }
  }
  os << "largest amp is " << largestName 
     << " with value " << largestValue 
     << endl;
}


void FitAmpList::printAllAmps(std::ostream& os){
  bool dbThis=false;
  if(_fitAmps.empty()) createAllAmps();

  std::string largestName = "none";
  if(dbThis) cout << "Debug mode for  FitAmpList::printAllAmps" << endl;

  if(0 == getEvent()){
    os << "FitAmpList::printAllAmps(): no Event, no Amp"
       << endl;
    return;
  }
  
  os << "FitAmpList::printAllAmps()\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    os << "\n\t" << (_fitAmps[i])->theDecay().oneLiner()
       << " \t" << (_fitAmps[i])->getVal()
       << endl;
  }
}
void FitAmpList::print(std::ostream& os) const{
   os << "FitAmpList::print\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    os << "\n\t" << (_fitAmps[i])->theDecay().oneLiner()
       << endl;
  }
}
void FitAmpList::printNonZero(std::ostream& os) const{
   os << "FitAmpList::printNonZero\n====================";

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if((_fitAmps[i])->isZero()) continue;
    os << "\n\t" << (_fitAmps[i])->theDecay().oneLiner()
       << endl;
  }
}

void FitAmpList::setAllAmpsTo(std::complex<double> z){
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    (_fitAmps[i])->FitAmpPhase().set(z);
  }
}

DalitzBoxSet FitAmpList::makeBoxes(IGetRealEvent<IDalitzEvent>* pdf
                                   , double nSigma
                                   ){
  if(0 == getEvent()){
    cout << "NOTE in FitAmpList::makeBoxes()"
         << " I got called, but there is no event."
         << " Need at least one event to determine final state."
         << " Alternatively pass me the pattern as an argument:"
         << " makeBoxes(pattern);"
         << endl;
    cout << " event record: " << getEventRecord()->size() << endl;
    return DalitzBoxSet( 0 ) ;
  }
  
  DalitzEventPattern thePattern = getEvent()->eventPattern();
  // this assumes that the final state is the same
  // for all events in list. No point, really, in
  // adding amplitudes for different final states.
  
  return makeBoxes(thePattern, pdf, nSigma);
}

DalitzBoxSet FitAmpList::makeBoxes(const DalitzEventPattern& pat
				   , IGetRealEvent<IDalitzEvent>* pdf
				   , double nSigma){
  DalitzBoxSet boxes;
  DalitzBox phaseSpaceBox(pat);
  boxes.add(phaseSpaceBox);

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    boxes.add( (_fitAmps[i])->MakeBoxes(pat, nSigma) );
  }
  boxes.setPDF(pdf);
  return boxes;
}


DalitzBWBoxSet FitAmpList::makeBWBoxes(IGetRealEvent<IDalitzEvent>* pdf
				       , TRandom* rnd){
  if(0 == getEvent()){
    cout << "NOTE in FitAmpList::makeBWBoxes()"
	 << " I got called, but there is no event."
	 << " Need at least one event to determine final state."
	 << " Alternatively pass me the pattern as an argument:"
	 << " makeBoxes(pattern);"
	 << endl;
    cout << " event record: " << getEventRecord()->size() << endl;
    DalitzBWBoxSet dummyBox(pdf, rnd);
    return dummyBox;
  }

  DalitzEventPattern thePattern = getEvent()->eventPattern();
  // this assumes that the final state is the same
  // for all events in list. No point, really, in
  // adding amplitudes for different final states.

  return makeBWBoxes(thePattern, pdf, rnd);
}

DalitzBWBoxSet FitAmpList::makeBWBoxes(const DalitzEventPattern& pat
				       , IGetRealEvent<IDalitzEvent>* pdf
				       , TRandom* rnd
				      ){
  DalitzBWBoxSet boxes(pdf, rnd);
  //  DalitzBox phaseSpaceBox(pat);
  //  boxes.add(phaseSpaceBox);

  for(unsigned int i=0; i<_fitAmps.size(); i++){
    DalitzBWBoxSet oneAmpsBox((_fitAmps[i])->MakeBWBoxes(pat, rnd));
    boxes.add(oneAmpsBox);
  }
  return boxes;
}


void FitAmpList::multiply(double r){
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(0 != (_fitAmps[i])){
      (_fitAmps[i])->multiply(r);
    }
  }
}
void FitAmpList::multiply(const std::complex<double>& z){
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(0 != (_fitAmps[i])){
      (_fitAmps[i])->multiply(z);
    }
  }
}
void FitAmpList::multiply(const MINT::counted_ptr<MINT::IReturnComplex> irc){
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(0 != (_fitAmps[i])){
      (_fitAmps[i])->multiply(irc);
    }
  }
}

void FitAmpList::deleteAll(){
  for(unsigned int i=0; i<_fitAmps.size(); i++){
    if(0 != (_fitAmps[i])){
      delete (_fitAmps[i]);
    }
  }
  _fitAmps.clear();
}

void FitAmpList::setEfficiency(counted_ptr<IGetDalitzEvent> eff){
  _efficiency=eff;
  _efficiency->setDaddy(this);
}
double FitAmpList::efficiency(){
  if(0 == _efficiency) return 1.0;
  double eff = _efficiency->RealVal();
  if(eff < 0) return 0;
  return eff;
}

FitAmpList::~FitAmpList(){
  deleteAll();
}

FitAmpList& FitAmpList::operator+=(const FitAmpList& other){
  add(other);
  return *this;
}
FitAmpList FitAmpList::operator+(const FitAmpList& rhs) const{
  FitAmpList fas(*this);
  fas.add(rhs);
  return fas;
}


FitAmpList& FitAmpList::operator*=(double r){
  multiply(r);
  return *this;
}
FitAmpList& FitAmpList::operator*=(const complex<double>& z){
  multiply(z);
  return *this;
}
FitAmpList& FitAmpList::operator*=(const counted_ptr<IReturnComplex>& irc){
  multiply(irc);
  return *this;
}

FitAmpList FitAmpList::operator*(double r) const{
  FitAmpList fas(*this);
  fas.multiply(r);
  return fas;
}
FitAmpList FitAmpList::operator*(const complex<double>& z) const{
  FitAmpList fas(*this);
  fas.multiply(z);
  return fas;
}
FitAmpList FitAmpList::operator*(const counted_ptr<IReturnComplex>& irc) const{
  FitAmpList fas(*this);
  fas.multiply(irc);
  return fas;
}


FitAmpList operator*(double r, const FitAmpList& rhs){
  FitAmpList fas(rhs);
  fas.multiply(r);
  return fas;
}
FitAmpList operator*(const complex<double>& z, const FitAmpList& rhs){
  FitAmpList fas(rhs);
  fas.multiply(z);
  return fas;
}
FitAmpList operator*(const counted_ptr<IReturnComplex>& irc
		     , const FitAmpList& rhs){
  FitAmpList fas(rhs);
  fas.multiply(irc);
  return fas;
}



//
