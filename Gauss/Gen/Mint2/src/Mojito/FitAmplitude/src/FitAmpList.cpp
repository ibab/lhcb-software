// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpList.h"

#include "Mint/FitAmplitude.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmpIncoherentSum.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/MultiTopHat.h"

#include <iostream>

using namespace std;
using namespace MINT;

FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , const char* fname
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : FitAmpListBase()
  , _pat(pat)
  , _minuitParaSet(pset)
  , _opt(opt)
{
  if(0 != fname){
    _paraFName = fname;
  }else{
    _paraFName.clear();
  }
  
  createAllAmps(pat, prefix);
}

FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , MinuitParameterSet* pset
		       , const std::string& prefix
		       , const std::string& opt
		     )
  : FitAmpListBase()
  , _pat(pat)
  , _minuitParaSet(pset)
  , _opt(opt)
{

  cout << "pset pointer in FitAmpList::FitAmpList " << getMPS() << endl;
  
  _paraFName.clear();
  
  createAllAmps(pat, prefix);
}
FitAmpList::FitAmpList(const DalitzEventPattern& pat
		       , const std::string& prefix
		       , const std::string& opt
		       )
  : FitAmpListBase()
  , _pat(pat)
  , _minuitParaSet(0)
  , _opt(opt)
{
  
  _paraFName.clear();
  
  createAllAmps(pat, prefix);
}

FitAmpList::FitAmpList(const FitAmpList& other)
  : FitAmpListBase(other)
  , _pat(other._pat)
  , _paraFName(other._paraFName)
  , _minuitParaSet(other._minuitParaSet)
  , _opt(other._opt)
{
}

FitAmpList& FitAmpList::operator=(const FitAmpList& other){
  if(&other == this) return *this;
  
  (FitAmpListBase)(*this) = (FitAmpListBase) (other);
  _pat           = other._pat;
  _paraFName     = other._paraFName;
  _minuitParaSet = other._minuitParaSet;
  _opt           = other._opt;
  return *this;
}


MINT::MinuitParameterSet* FitAmpList::getMPS(){
  if(0 == _minuitParaSet) _minuitParaSet = MinuitParameterSet::getDefaultSet();
  return _minuitParaSet;
}

counted_ptr<FitAmpListBase> FitAmpList::GetCloneSameFitParameters() const{ 
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

counted_ptr<FitAmpListBase> FitAmpList::GetCPConjugateSameFitParameters() const{
  bool dbThis=false;
  if(dbThis) cout << "FitAmpList::GetCPConjugateSameFitParameters()" << endl;

  counted_ptr<FitAmpList> newList = this->GetCloneSameFitParameters();
  newList->CPConjugateSameFitParameters();
  return newList;
}

bool FitAmpList::createAllAmps(const DalitzEventPattern& thePattern
			      , const std::string& prefix){
  bool dbThis=false;

  //  cout << "pset pointer in FitAmpList::createAllAmps " << getMPS() << endl;

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

    if(A_is_in_B("TopHats", it->second.lopt())){
      cout << "making TopHats" << endl;
      NamedParameter<double> TopHatsLimits("TopHatsLimits");
      if(TopHatsLimits.size() < 2){
	cout << "error when setting TopHatsLimits: need at least 2 limits, have "
	     << TopHatsLimits.size() << endl;
	continue;
      }
      MultiTopHat mth;
      for(unsigned int i=1; i < TopHatsLimits.size(); i++){
	double lo = TopHatsLimits.getVal(i-1);
	double hi = TopHatsLimits.getVal(i);
	
	cout << "TopHatLimits: " << lo << ", " << hi << endl;
	AmpInitialiser ai(it->second);
	ai.setNumOpts(lo, hi);
	cout << "ai set " << endl;
	//success &= addAmplitude(new FitAmplitude(prefix + it->first + "_" + anythingToString(i)
	success &= mth.addAmplitude(new FitAmplitude((prefix + it->first + "_" + anythingToString((int) i)).c_str()
						     , ai
						     , fnamePtr
						     , getMPS()
						     )
				    );
      }
      addAsList(mth);
    }else{
      //cout << "making a normal amplitude " << it->first << endl;
      success &= addAmplitude(new FitAmplitude(prefix + it->first
					       , it->second
					       , fnamePtr
					       , getMPS()
					       )
			      );
      
    }
    if(! success){
      cout << "ERROR in FitAmpList::createAllAmps!"
	   << "\n   > failed to create FitAmplitude"
	   << "\n   > for decay name " << it->first
	   << "\n   > \"new\" returned zero."
	   << "\n   > I'll keep going and try to"
	   << "\n   > create the other amplitudes."
	   << endl;
    }

  }
  return success;
}

void FitAmpList::printLargestAmp(IDalitzEvent& evt, std::ostream& os){
  bool dbthis=false;
  if(_fitAmps.empty()) createAllAmps(evt.eventPattern());
  FitAmpListBase::printLargestAmp(evt, os);
}


void FitAmpList::printAllAmps(IDalitzEvent& evt, std::ostream& os){
  bool dbThis=false;
  if(_fitAmps.empty()) createAllAmps(evt.eventPattern());  
  FitAmpListBase::printAllAmps(evt, os);
}

void FitAmpList::printNonZeroWithValue(IDalitzEvent& evt, std::ostream& os){
  bool dbThis=false;
  if(_fitAmps.empty()) createAllAmps(evt.eventPattern());
  FitAmpListBase::printNonZeroWithValue(evt, os);
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

std::ostream& operator<<(std::ostream& os, const FitAmpList& fal){
  fal.print(os);
  return os;
}

//
