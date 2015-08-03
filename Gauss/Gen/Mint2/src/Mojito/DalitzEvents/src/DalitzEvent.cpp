// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <cmath>
#include <algorithm>

#include "Mint/DalitzEvent.h"
#include "Mint/Utils.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include "TRandom.h"
#include "TGenPhaseSpace.h"
#include "TMatrixDSym.h"
#include "TNtupleD.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"

using namespace std;
using namespace MINT; // for Utils.h

const char DalitzEvent::prtNameChars[] = { '+', '-', '\0' };
const char DalitzEvent::ntpNameChars[] = { '#', '~', '\0' };

long int DalitzEvent::_eventCounter=0;

long int DalitzEvent::_rememberVectorCounter=0;
long int  DalitzEvent::assignUniqueRememberNumber(){
  return _rememberVectorCounter++;
}

DalitzEvent::DalitzEvent()
  : _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _permutationIndex(0)
  , _perm()
{
  _eventCounter++;
}
DalitzEvent::DalitzEvent(const DalitzEventPattern& pat
			 , const std::vector<TLorentzVector>& mumAndDgtrs_p4
			 )
  : _pat(pat)
  , _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _s(pat.size())
  , _t(pat.size())
  , _permutationIndex(0)
  , _perm(pat)
{
  setMomenta(mumAndDgtrs_p4);
  if(_pat.size() < 3 || _pat.size() != _p.size()) shoutAndKill();
  resetST();
  _eventCounter++;
}

DalitzEvent::DalitzEvent(const DalitzEventPattern& pat
			 , double t01_in
			 , double s12_in, double s23_in, double s34_in
			 , double t40_in
			 )
  : _pat(pat)
  , _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _s(pat.size())
  , _t(pat.size())
  , _permutationIndex(0)
  , _perm(pat)
{
  bool dbThis=false;

  if(_pat.size() != 5){
    cout << "DalitzEvent::DalitzEvent ERROR "
	 << " event pattern " << pat
	 << " doesn't fit 4-body constructor"
	 << endl;
    throw "shit happens";
  }
  Calculate4BodyProps c4bp(t01_in, s12_in, s23_in, s34_in, t40_in
			   , _pat[0].mass()
			   , _pat[1].mass(), _pat[2].mass()
			   , _pat[3].mass(), _pat[4].mass()
			   );
  if(dbThis){
    cout << " c4bp phase space " << c4bp.phaseSpaceFactor() << endl;
  }
  std::vector<TLorentzVector> p4List(5);
  if(c4bp.phaseSpaceFactor() > 0){ 
    for(int i=0; i<=4; i++){
      p4List[i] = c4bp.pVec(i);
      if(dbThis)cout << "p4List[" << i << "] " << p4List[i] << endl;
    }
  }else{
    for(int i=0; i<=4; i++){
      TLorentzVector p4(-9999, -9999, -9999, -9999);
      p4List[i] = p4;
    }
  }
  setMomenta(p4List);
  if(_pat.size() < 3 || _pat.size() != _p.size()) shoutAndKill();
  resetST();

  if(dbThis){
    cout << "constructed event from this pattern: " << _pat << "\n and "
	 << t01_in << ", " << s12_in << ", " << s23_in << ", " << s34_in << ", " << t40_in
	 << endl;
    
    cout << " and got out: "
	 << t_intern(0,1) << ", " << s_intern(1,2) << ", " << s_intern(2,3)
	 << ", " << s_intern(3,4) << ", " << t_intern(4,0)
	 << endl;
  }
  _eventCounter++;
}


DalitzEvent::DalitzEvent(const DalitzEventPattern& pat
			 , const std::vector<TVector3>& mumAndDgtrs_p3
			 )
  : _pat(pat)
  , _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _s(pat.size())
  , _t(pat.size())
  , _permutationIndex(0)
  , _perm(pat)
{
  setMomenta(mumAndDgtrs_p3);
  if(_pat.size() < 3 || _pat.size() != mumAndDgtrs_p3.size()) shoutAndKill();
  resetST();
  _eventCounter++;
}
DalitzEvent::DalitzEvent(const DalitzEventPattern& pat)
  : _pat(pat)
  , _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _s(pat.size())
  , _t(pat.size())
  , _permutationIndex(0)
  , _perm(pat)
{
  if (_pat.size() < 3) shoutAndKill();
  _p.resize(_pat.size());
  resetST();
  _eventCounter++;
} // generate or add momemta later.

DalitzEvent::DalitzEvent(const DalitzEventPattern& pat, TRandom* rnd)
  : _pat(pat)
  , _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _s(pat.size())
  , _t(pat.size())
  , _permutationIndex(0)
  , _perm(pat)
{
  if (_pat.size() < 3) shoutAndKill();
  resetST();
  generateThisToPhaseSpace(rnd);
  _eventCounter++;
}

DalitzEvent::DalitzEvent(const IDalitzEvent* other)
  : _pat(other->eventPattern())
    //  , _p(other.)
  , _rememberPhaseSpace(other->phaseSpace())
  , _rememberComplexFast()
  , _rememberDoubleFast()
  , _aValue(other->getAValue())
  , _weight(other->getWeight())
  , _generatorPdfRelativeToPhaseSpace(other->getGeneratorPdfRelativeToPhaseSpace())
    //  , _s()
    //, _t(other._t)
  , _permutationIndex(0)
  , _perm(other->eventPattern())
{
  int np = other->eventPattern().size();
  _p.resize(np);
  for(int i=0; i < np; i++) _p[i] = other->p(i);
  resetST();
  _eventCounter++;
}

DalitzEvent::DalitzEvent(const IDalitzEvent& other)
  : _pat(other.eventPattern())
  , _rememberPhaseSpace(other.phaseSpace())
  , _rememberComplexFast()
  , _rememberDoubleFast()
  , _aValue(other.getAValue())
  , _weight(other.getWeight())
  , _generatorPdfRelativeToPhaseSpace(other.getGeneratorPdfRelativeToPhaseSpace())
  , _permutationIndex(0)
  , _perm(other.eventPattern())
{
  int np = other.eventPattern().size();
  _p.resize(np);
  for(int i=0; i < np; i++) _p[i] = other.p(i);
  resetST();
  _eventCounter++;
}



DalitzEvent::DalitzEvent(const DalitzEvent* other)
  : IDalitzEvent() // the compiler wants it.
  , _pat(other->_pat)
  , _p(other->_p)
  , _rememberPhaseSpace(other->_rememberPhaseSpace)
  , _rememberComplexFast(other->_rememberComplexFast)
  , _rememberDoubleFast(other->_rememberDoubleFast)
  , _aValue(other->_aValue)
  , _weight(other->_weight)
  , _generatorPdfRelativeToPhaseSpace(other->_generatorPdfRelativeToPhaseSpace)
  , _s(other->_s)
  , _t(other->_t)
  , _sijMap(other->_sijMap)
  , _permutationIndex(other->_permutationIndex)
  , _perm(other->_perm)
{
  bool dbThis=false;
  if(dbThis) cout << " copy constructor of DalitzEvent called."
		  << " weight before " << other->getWeight()
		  << " and after " << this->getWeight()
		  << endl;
  _eventCounter++;
}
DalitzEvent::DalitzEvent(const DalitzEvent& other)
  : IDalitzEvent() // the compiler wants it.
  , _pat(other._pat)
  , _p(other._p)
  , _rememberPhaseSpace(other._rememberPhaseSpace)
  , _rememberComplexFast(other._rememberComplexFast)
  , _rememberDoubleFast(other._rememberDoubleFast)
  , _aValue(other._aValue)
  , _weight(other._weight)
  , _generatorPdfRelativeToPhaseSpace(other._generatorPdfRelativeToPhaseSpace)
  , _s(other._s)
  , _t(other._t)
  , _sijMap(other._sijMap)
  , _permutationIndex(other._permutationIndex)
  , _perm(other._perm)
{
  bool dbThis=false;
  if(dbThis) cout << " copy constructor of DalitzEvent called."
		  << " weight before " << other.getWeight()
		  << " and after " << this->getWeight()
		  << endl;
  _eventCounter++;
}
DalitzEvent::DalitzEvent(TNtupleD* ntp)
  : _rememberPhaseSpace(-9999.)
  , _aValue(-9999.)
  , _weight(1)
  , _generatorPdfRelativeToPhaseSpace(1)
  , _permutationIndex(0)
{
  if(! fromTree(ntp)){
    cout << "ERROR in DalitzEvent constructor from ntuple"
	 << " something went wrong!"
	 << endl;
  }
  _eventCounter++;
}

DalitzEvent::~DalitzEvent(){
  _eventCounter--;
  //  cout << "DalitzEvent destroyed\n" << endl;
}

IDalitzEvent* DalitzEvent::clone() const{
  return new DalitzEvent(*this);
}
double DalitzEvent::getWeight() const{
  return _weight;
}
void DalitzEvent::setWeight(double w){
  _weight = w;
}
double DalitzEvent::getGeneratorPdfRelativeToPhaseSpace() const{
  return _generatorPdfRelativeToPhaseSpace;
}
void DalitzEvent::setGeneratorPdfRelativeToPhaseSpace(double gpdf){
  _generatorPdfRelativeToPhaseSpace = gpdf;
}
double DalitzEvent::m(unsigned int i) const{
  // easy access to the nominal masses
  if(i >= _pat.size()){
    cout << "ERROR in DalitzEvent::m(unsigned int i)"
	 << " index i= " << i << " out of range!"
	 << " allowed range: [0," << _pat.size()-1 << "]"
	 << " returning -9999"
	 << endl;
    return -9999;
  }
  return _pat[i].mass();
}
double DalitzEvent::m2(unsigned int i) const{
  if(i >= _pat.size()){
    cout << "ERROR in DalitzEvent::m2(unsigned int i)"
	 << " index i= " << i << " out of range!"
	 << " allowed range: [0," << _pat.size()-1 << "]"
	 << " returning -9999"
	 << endl;
    return -9999;
  }  
  return m(i)*m(i);
}

TLorentzVector DalitzEvent::p_dgtr_sum() const{
  TLorentzVector pd(0.0, 0.0, 0.0, 0.0);

  for(unsigned int i=1; i< _p.size(); i++){
    pd += _p[i];
  }
  return pd;
}
    
bool DalitzEvent::kinematicallyAllowed(double epsilon) const{
  bool dbThis=false;

  TLorentzVector pdiff = (p(0) - p_dgtr_sum());
  TLorentzVector psum  = (p(0) + p_dgtr_sum());

  double diffsum = 0;
  double sumsum  = 0;
  for(unsigned int i=0; i<4; i++){
    diffsum += pdiff[i]*pdiff[i];
    sumsum  += psum[i] *psum[i];
  }
  double avsum = sumsum/2.0;

  if(diffsum > epsilon*epsilon * avsum){
    if(dbThis){
      cout << "DalitzEvent::kinematicallyAllowed? "
	   << " failing diffsum: " << diffsum
	   << " max allowed: " << epsilon*epsilon*avsum << endl;
    } 
    return false;
  }

  for(unsigned int i = 0; i<_pat.size(); i++){
    double mreco = p(i).M();
    if(mreco < 0){
      if(dbThis) cout << "failing mreco " << mreco << " < 0" << endl;
      return false;
    }
    double avmass = (mreco + m(i))/2.0;
    if( fabs(mreco - m(i)) > epsilon*avmass ){
      if(dbThis){
	cout << " failing mreco - m(i) fabs(" 
	     << mreco << " - " << m(i) << ") = " 
	     << fabs(mreco - m(i)) << " > " << epsilon*avmass
	     << endl;
      }
      return false;
    }
  }
  return true;
}
  

double DalitzEvent::phaseSpace() const{
  if(nDgtr() <= 2){
    _rememberPhaseSpace = 1;
    return 1;
  }
  if(_p.size() != _pat.size()){
    cout << "ERROR in DalitzEvent::phaseSpace: inconsistency!"
	 << " pattern array size != momentum array size"
	 << " " << _pat.size() << " != " << _p.size()
	 << ". returning -9999"
	 << endl;
    return -9999;
  }

  if(_rememberPhaseSpace > -9998){
    //    cout << " remembering... " << _rememberPhaseSpace << endl;
    return _rememberPhaseSpace;
  }

  if(nDgtr() == 3){
    _rememberPhaseSpace = phaseSpace3(); // one or zero
    return _rememberPhaseSpace;
  }else if(nDgtr() == 4){
    _rememberPhaseSpace = phaseSpace4();
    return _rememberPhaseSpace;
  }else{
    cout << "ERROR in DalitzEvent::phaseSpace inconsistency!"
	 << "\n   > Sorry, don't know how to calculate phase-space"
	 << " factor for nDgtr = " << nDgtr()
	 << "\n   >  - can only do up to nDgtr <= 4"
	 << ". returning -9999"
	 << endl;
    return -9999;
  }
}

void DalitzEvent::generateThisToPhaseSpace(TRandom* rnd){
  generateThisToPhaseSpace(0.0, rnd);
}
void DalitzEvent::generateThisToPhaseSpace(double p, TRandom* rnd){
  TVector3 p3(0,0,p);
  generateThisToPhaseSpace(p3, rnd);
}
void DalitzEvent::generateThisToPhaseSpace(const TVector3& mumsp3
					   , TRandom* rnd){
  bool dbthis=false;
  if(dbthis) cout << "got called" << endl;
  TRandom* remember_gRandom = gRandom;
  bool needToReset_gRandom=false;
  if(0 != rnd){
    needToReset_gRandom=true;
    gRandom = rnd; // horrible fix because
    // TGenPhaseSpace can only use gRandom
  }
  if(dbthis) cout << "got so far" << endl;
  TGenPhaseSpace gps; // uses gRandom, so no worries
  // about re-starting with the same seed (won't happen), 
  // but worries about what kind of
  // random number generator gRandom is.

  TLorentzVector mumsP4;
  mumsP4.SetXYZM(mumsp3.X()
		 , mumsp3.Y()
		 , mumsp3.Z()
		 , _pat[0].mass());

  Double_t* mdgt = new Double_t[nDgtr()];
  if(dbthis) cout << "booked " << nDgtr() << " daughters " << endl;
  for(int i=1; i<= nDgtr(); i++){
    mdgt[i-1] = this->m(i);
  }
  
  gps.SetDecay(mumsP4, nDgtr() , mdgt);
  double maxWeight = 1.0;//gps.GetWtMax();
  double weight=0;
  if(dbthis) cout << " maxWeight " << maxWeight << endl;
  //normal:
  do{
    weight=gps.Generate();
  }while(weight < gRandom->Rndm() * maxWeight);
  // for debug:
  //  weight=gps.Generate();

  if(needToReset_gRandom){
    gRandom = remember_gRandom;
  }
  if(weight > maxWeight){
    cout << "ERROR in DalitzEvent::generateThisToPhaseSpace"
	 << " weight = " << weight << " > " << maxWeight << " = maxWeight"
	 << "\n    > this is a programming error that needs to be fixed."
	 << endl;
    throw "...so I got maxWeight wrong.";
  }
  if(dbthis) cout << " this weight " << weight << endl;
  if(dbthis) cout << " max weight " << gps.GetWtMax() << endl;

  _p.resize(nDgtr()+1);
  if(dbthis) cout << "generated event" << endl;
  setP(0, mumsP4);
  if(dbthis)cout << "set p0" << endl;
  for(unsigned int i=1; i< _pat.size(); i++){
    if(dbthis)cout << "setting P (" << i << ") " 
		   << (*(gps.GetDecay(i-1)))
		   << "..." << endl;
    setP(i, (*(gps.GetDecay(i-1))));
    if(dbthis)cout << " and look what I've set: " << p(i) << endl;
  }
  delete[] mdgt;
  _weight = 1.0;
  _generatorPdfRelativeToPhaseSpace = 1.0;
  return;
}

const TLorentzVector& DalitzEvent::p_intern(unsigned int i) const{
  if(i >= _p.size()){
    cout << "in TLorentzVector& DalitzEvent::p(unsigned int i):"
	 << " index out of range. DalitzEventPattern:"
	 << "\n  > \"" << eventPattern() << "\""
	 << "\n  > num mother + daughters = " << _p.size()
	 << endl;
    throw "index out or range in  TLorentzVector& DalitzEvent::p(unsigned int i)";
  }
  return _p[i];
}

TLorentzVector& DalitzEvent::p_intern(unsigned int i){
  if(i >= _p.size()){
    cout << "in TLorentzVector& DalitzEvent::p(unsigned int i):"
	 << " index out of range. DalitzEventPattern:"
	 << "\n  > \"" << eventPattern() << "\""
	 << "\n  > num mother + daughters = " << _p.size()
	 << endl;
    throw "index out or range in  TLorentzVector& DalitzEvent::p(unsigned int i)";
  }
  return _p[i];
}

double DalitzEvent::s_intern(unsigned int i, unsigned int j) const{
  if(_s[i][j] < 0) _s[i][j] = (p_intern(i) + p_intern(j)).Mag2();
  return _s[i][j];
}
double DalitzEvent::t_intern(unsigned int i, unsigned int j) const{
  if(_t[i][j] < 0) _t[i][j] = (p_intern(i) - p_intern(j)).Mag2();
  return _t[i][j];
}
double DalitzEvent::sijMap_intern(const std::vector<int>& intern_indices) const{
  bool successFlag=false;
  double returnVal = MINT::keyFinder(intern_indices, _sijMap
				     , (double) 0, successFlag);
  if(! successFlag){
    returnVal = evalsij_intern(intern_indices);
    _sijMap[intern_indices] = returnVal;
  }
  return returnVal;
}
double DalitzEvent::sijMap(const std::vector<int>& indices) const{
  vector<int> intern_indices(indices);
  for(unsigned int i=0; i < intern_indices.size(); i++){
    intern_indices[i] = currentPermutation()[ indices[i] ];
  }
  return sijMap_intern(intern_indices);
}

const TLorentzVector& DalitzEvent::p(unsigned int i) const{
  bool dbThis=false;
  if(dbThis){
    cout << "p(" << i << ") called. Current permutation: "
	 << currentPermutation()[i]
	 << endl;
  }
  return p_intern(currentPermutation()[i]);
}
TLorentzVector& DalitzEvent::p(unsigned int i){
  bool dbThis=false;
  if(dbThis){
    cout << "p(" << i << ") called. Current permutation: "
	 << currentPermutation()[i]
	 << endl;
  }
  return p_intern(currentPermutation()[i]);
}
double DalitzEvent::s(unsigned int i, unsigned int j) const{
  return s_intern(currentPermutation()[i], currentPermutation()[j]);
}
double DalitzEvent::t(unsigned int i, unsigned int j) const{
  return t_intern(currentPermutation()[i], currentPermutation()[j]);
}


double DalitzEvent::evalsij_intern(const std::vector<int>& 
				   intern_indices) const{
  TLorentzVector psum(0.0, 0.0, 0.0, 0.0);
  for(unsigned int i=0; i< intern_indices.size(); i++){
    //cout << " p(" << i << ") " << p(indices[i]);
    psum += p_intern(intern_indices[i]);
  }
  return psum.Mag2();
}

double DalitzEvent::sij(const std::vector<int>& indices) const{
  bool dbThis=false;

  if(1 == indices.size()){
    return p(indices[0]).Mag2();
  }else if(2 == indices.size()){
    return s(indices[0], indices[1]);
  }else{
    if(dbThis){
      TLorentzVector psum(0.0, 0.0, 0.0, 0.0);
      for(unsigned int i=0; i< indices.size(); i++){
	//cout << " p(" << i << ") " << p(indices[i]);
	psum += p(indices[i]);
      }
      cout << "DalitzEvent sij: compare "  << psum.Mag2()
	   << " = " << sijMap(indices) << endl;
    }
    return sijMap(indices);
  }
}

double DalitzEvent::sijMin(const std::vector<int>& indices) const{
  return _pat.sijMin(indices);
}
double DalitzEvent::sijMax(const std::vector<int>& indices) const{
  return _pat.sijMax(indices);
}

void DalitzEvent::setMothers3Momentum(const TVector3& mp3){
  TLorentzVector newMum4;
  newMum4.SetXYZM(mp3.X(), mp3.Y(), mp3.Z(), _p[0].M());
  
  TVector3 boostToOldMumsRest(- _p[0].BoostVector());
  TVector3 boostToNewMumsLab(newMum4.BoostVector());
  for(unsigned int i=0; i < _p.size(); i++){
    _p[i].Boost(boostToOldMumsRest); // to D0 rest frame
    _p[i].Boost(boostToNewMumsLab); //  to lab frame with new D0 momentum   
  }
  
}


Calculate4BodyProps DalitzEvent::makeCalculate4BodyProps() const{
  if(nDgtr() != 4){
    cout << "ERROR in DalitzEvent::makeCalculate4BodyProps()"
	 << "\n   > You can't expect a sensible Calculate4BodyProps"
	 << " Object for a " << nDgtr() << " body decay!"
	 << endl;
    return Calculate4BodyProps(0,0,0,0,0, 0,0,0,0,0);
  }
  return Calculate4BodyProps(t(0,1), s(1,2), s(2,3), s(3,4), t(4,0)
			     , m(0), m(1), m(2), m(3), m(4)
			     );
}

void DalitzEvent::print(std::ostream& os) const{
  os << "Dalitz event for pattern "
     << _pat;
  for(unsigned int i=0; i<_p.size(); i++){
    cout << "\n\t p(" << i << ") " << p(i);
  }
  os << "\n\t m(i) ";
  for(unsigned int i=0; i<_p.size(); i++){
    cout << m(i) << " ; ";
  }
  if(nDgtr() == 4){
    os << "\n\t sij :"
       << t(0,1) << ", " << s(1,2) << ", " << s(2,3) 
       << ", " << s(3,4) << ", " << t(4, 0);
  }
  os << "\n\t ps = " << this->phaseSpace();
  os << "\t weight = " << this->getWeight();
  os << endl;
}


// private stuff:

bool DalitzEvent::shoutAndKill(){
  cout << "FATAL ERROR in DalitzEvent:"
       << "\n  > pattern: \"" << _pat << "\""
       << "\n  > 4-vector list size: " << _p.size()
       << endl;
  throw "probably insonsistent pattern in DalitzEvent";
}

void DalitzEvent::P_conjugateYourself(){
  // p-conjugates in mums restframe, but keeps old D momentum.

  resetST(); // shouldn't be necessary, but to be save.
  if(_p.empty()) return;
  TVector3 mums3Momentum(_p[0].X(), _p[0].Y(), _p[0].Z());
  for(unsigned int i=0; i < _p.size(); i++){
    _p[i].SetX( - _p[i].X() );
    _p[i].SetY( - _p[i].Y() );
    _p[i].SetZ( - _p[i].Z() );
  }
  setMothers3Momentum(mums3Momentum);
}
void DalitzEvent::C_conjugateYourself(){
  _pat = _pat.makeCPConjugate();
}
void DalitzEvent::CP_conjugateYourself(){
  P_conjugateYourself();
  C_conjugateYourself();
}

bool DalitzEvent::resetST(){ 
  // resets (possibly previously calculated) values for sij, tij

  _s.resize(_pat.size());
  for(unsigned int i=0; i< _s.size(); i++){
    _s[i].clear();
    _s[i].resize(_pat.size());
    for(unsigned int j=0; j< _s[i].size(); j++){
      _s[i][j] = -1;
    }
  }

  _t.resize(_pat.size());
  for(unsigned int i=0; i< _t.size(); i++){
    _t[i].clear();
    _t[i].resize(_pat.size());
    for(unsigned int j=0; j< _t[i].size(); j++){
      _t[i][j] = -1;
    }
  }
  return true;
}

void DalitzEvent::setPermutationIndex(int i){
  if(i >= numPermutations()) return;
  _permutationIndex=i;
}
int DalitzEvent::numPermutations() const{
  return _perm.size();
}

bool DalitzEvent::setMomenta(const std::vector<TLorentzVector>& p4List){
  _rememberPhaseSpace = -9999;
  if(_pat.size() != p4List.size()){
    return false;
  }
  _p.resize(p4List.size());
  for(unsigned int i=0; i<p4List.size(); i++){
    _p[i] = p4List[i];
  }
  return true;
}

bool DalitzEvent::setMomenta(const std::vector<TVector3>& p3List){
  _rememberPhaseSpace = -9999;

  _p.resize(p3List.size());
  if(_pat.size() != p3List.size()){
    return false;
  }
  for(unsigned int i=0; i< _pat.size(); i++){
    const TVector3& p3 = p3List[i];
    double m = _pat[i].mass();
    _p[i].SetXYZM(p3.X(), p3.Y(), p3.Z(), m);
  }
  return true;
}

void DalitzEvent::setP(unsigned int i, const TLorentzVector& p4){
  _rememberPhaseSpace = -9999;
  if(i >= _p.size()){
    _p.resize(i+1);
    //    cout << "had to resize" << endl;
  }
  _p[i] = p4;
}

double DalitzEvent::BDet()const{
  /*
  cout << " for sij " << t(0,1) << ", " << s(1,2) << ", " << s(2,3)
       << ", " << s(3,4) << ", " << t(4,0) << endl;
  cout << "\n " << m2(0) << ", " << m2(1) << ", " << m2(2)
       << ", " << m2(3) << m2(4) << endl;
  */

  if(nDgtr() < 4){
    cout << "ERROR in DalitzEvent::BDet(): nDgtr() = "
	 << nDgtr() << " < 4. How did this happen?"
	 << endl;
    return +9999;
  }

  TMatrixDSym B(6);
  B(0,0)=0;
  B(1,0)=B(0,1)=m2(2); B(1,1) = 0;
  B(2,0)=B(0,2)=s(2,3) ; B(2,1)=B(1,2) = m2(3); B(2,2)=0;
  B(3,0)=B(0,3)=t(0,1) ; B(3,1)=B(1,3) = s(3,4) ; B(3,2)=B(2,3)=m2(4); B(3,3)=0;
  B(4,0)=B(0,4)=m2(1); B(4,1)=B(1,4) = s(1,2) ; B(4,2)=B(2,4)=t(4,0) ; B(4,3)=B(3,4)=m2(0); B(4,4)=0;
  B(5,0)=B(0,5)=B(5,1)=B(1,5)=B(5,2)=B(2,5)=B(5,3)=B(3,5)=B(5,4)=B(4,5)=1; 
  B(5,5)=0;

  /*
  double Bdet = B.Determinant();
  cout << "Bdet = " << Bdet << endl;
  */

  return B.Determinant();
}
double DalitzEvent::Delta4() const{
  return -BDet()/16;
}
double DalitzEvent::phaseSpace4() const{ 
  bool dbThis=false;
  if(dbThis)cout << "\n\n called phaseSpace4() " << endl;
  if(nDgtr() < 4){
    cout << "ERROR in DalitzEvent::phaseSpace4: nDgtr() = "
	 << nDgtr() << " < 4. How did this happen?"
	 << endl;
    return -9999;
  }
  if(dbThis){
    Calculate4BodyProps c4bp(t_intern(0,1), s_intern(1,2), s_intern(2,3)
			     , s_intern(3,4), t_intern(4,0)
			     , m(0), m(1), m(2), m(3), m(4)
			     );
    
    cout << "4-body probs gets this phase space: "
	 << c4bp.phaseSpaceFactor() << endl;
    cout << " ... for t01 <<... = "
	 << t_intern(0,1) << ", " << s_intern(1,2) << ", " << s_intern(2,3)
	 << ", " << s_intern(3,4) << ", " << t_intern(4,0)
	 << endl;
  }

  // Note that this calculation assumes that this is a "physical" event.
  // It doesn't check if it is outside the physically allowed
  // phase space area, for which there are more requirements than
  // only Delta4 < 0.
  double invPhsp2 = -Delta4();
  if(invPhsp2 <= 0) return 0;
  double phsp = pi*pi/(32.0*m(0)*m(0)) *  1./sqrt(invPhsp2);
  if(dbThis) cout << "  ... I get: phsp = " << phsp << endl;
  if(phsp < 0) return 0;
  else return phsp;
}

double DalitzEvent::phaseSpace3(double epsilon) const{
  bool dbThis=false;
  // return either one or zero.
  // one if it's kinematically allowed, zero otherwise.
  if(kinematicallyAllowed(epsilon)){
    if(dbThis) {
      cout << "DalitzEvent::phaseSpace3(" << epsilon << ")"
	   << " returning 1" << endl;
    }
    //return pi*pi/(4.0*m(0)*m(0));
    return 1;
  }else{
    if(dbThis) {
      cout << "DalitzEvent::phaseSpace3(" << epsilon << ")"
	   << " returning 0" << endl;
    }
    return 0;
  }
}

std::string DalitzEvent::prtToNtpName(const std::string& s_in){
  std::string s(s_in);
  for(int i=0; prtNameChars[i] != '\0'; i++){
    replace(s.begin(), s.end(), prtNameChars[i], ntpNameChars[i]);
  }
  return s;
}
std::string DalitzEvent::ntpToPrtName(const std::string& s_in){
  std::string s(s_in);
  for(int i=0; ntpNameChars[i] != '\0'; i++){
    replace(s.begin(), s.end(), ntpNameChars[i], prtNameChars[i]);
  }
  return s;
}

std::string DalitzEvent::makeNtupleVarnames()const{
  std::string s="";
  if(eventPattern().empty()){
    return s;
  }

  for(unsigned int i=0; i< _pat.size(); i++){
    std::string name = _pat[i].name();
    name = "_" + anythingToString((int) i) + "_" + name;
    name = prtToNtpName(name);

    s += name + "_pdg:";
    s += name + "_E:";
    s += name + "_Px:";
    s += name + "_Py:";
    s += name + "_Pz";
    //    if(i != _pat.size() -1) s+= ":";
    s+= ":";
  }
  s += "weight:";
  s += "genPdf";
  return s;
}

int DalitzEvent::singleParticleNtpArraySize(){
  return 5;
}
bool DalitzEvent::fillNtupleVarArray(Double_t* array
				     , unsigned int arraySize)const{

  if(eventPattern().empty()){
    return false;
  }
  int counter=0;
  int maxCounter = (arraySize - 2) - (singleParticleNtpArraySize()-1);
  for(unsigned int i=0; i< _p.size() && counter < maxCounter; i++){
    array[counter++] = _pat[i];
    array[counter++] = _p[i].E();
    array[counter++] = _p[i].X();
    array[counter++] = _p[i].Y();
    array[counter++] = _p[i].Z();
  }
  array[counter++] = getWeight();
  array[counter++] = getGeneratorPdfRelativeToPhaseSpace();
  return true;
}
unsigned int DalitzEvent::ntupleVarArraySize() const{
  return _p.size() * singleParticleNtpArraySize() + 2;
}

/*
bool DalitzEvent::mapMe(const Permutation& mapping){
  bool s=true;
  s &= resetST();
  _pat = (DalitzEventPattern) mapping.mapOrder(_pat);
  std::vector<TLorentzVector> newP(mapping.mapOrder(_p));
  s &= setMomenta(newP);
  return s;
}
*/

bool DalitzEvent::fromTree(TTree* tree){
  bool dbThis=false;
  // figure out format: TNtuple or what?
  if(0 == tree) return 0;
  std::string treeName(tree->ClassName());
  if(dbThis) cout << "ClassName \"" << treeName << "\"" << endl;
  if(treeName == (std::string) "TNtupleD"){
    if(dbThis)cout << "DalitzEvent::fromTree: tree is TNtupleD" << endl;
    return fromNtuple((TNtupleD*)tree);
  }else{
    if(dbThis) cout << "DalitzEvent::fromTree: tree is ParasTree" << endl;
    return fromParasTree(tree);
  }
}
bool DalitzEvent::fromNtuple(TNtupleD* ntp){
  // assumes ntuple is set to the correct
  // entry (using ntp->GetEntry(int i))
  bool dbThis=false;
  if(dbThis) cout << "DalitzEvent::fromNtuple(" << ntp->GetName() << ") called" << endl;
  int arraySize = ntp->GetNvar();
  //  int arraySize = ntp->GetNbranches();
  if(dbThis) cout << " arraySize " << arraySize << endl;
  bool newFormat = false;
  if(0 == (arraySize-2)%singleParticleNtpArraySize() ){
	if(dbThis) cout << "New Format" << endl;
    newFormat = true; // includes weight & generatorPdf
  }else if( 0 == (arraySize)%singleParticleNtpArraySize()){
    newFormat = false; // excludes weight & generatorPdf
  }else{
    cout << "ERROR in DalitzEvent::fromNtuple length of array"
	 << " in ntuple = " << arraySize
	 << "\n   >  not a multiple of # entries per particle = "
	 << singleParticleNtpArraySize()
	 << endl;
    return false;
  }
  const Double_t* array = ntp->GetArgs();

  if(dbThis) cout << " array ptr " << array << endl;
  int counter = 0;
  int maxCounter = arraySize - (singleParticleNtpArraySize()-1);
  if(newFormat) maxCounter -= 2;
  int nParticles;
  if(newFormat){
    nParticles = (arraySize-2)/singleParticleNtpArraySize();
  }else{
    nParticles = arraySize/singleParticleNtpArraySize();
  }
  if(dbThis){
    cout << "nParticles = " << nParticles
	 << " arraySize = " << arraySize
	 << endl;
  }

  _pat.resize(nParticles);
  _p.resize(nParticles);
  for(int i=0; i< nParticles && counter < maxCounter; i++){
    if(dbThis) cout << "filling " << i << " th particle: " << endl;
    _pat[i] = nearestInt(array[counter++]);
    _p[i].SetE(array[counter++]);
    _p[i].SetX(array[counter++]);
    _p[i].SetY(array[counter++]);
    _p[i].SetZ(array[counter++]);
    
    // check units (should be MeV, but will recognise if its GeV and 
    // translate it to MeV)

    double units(1);
    double dMeV = abs(_pat[i].mass() - _p[i].M()*MeV);
    double dGeV = abs(_pat[i].mass() - _p[i].M()*GeV);
    if(dMeV < dGeV) units=MeV;
    else units=GeV;
    _p[i] *= units;
    
    if(dbThis){
      cout << "\t" << _pat[i] << " " << _p[i] 
	   << ", m= " << _p[i].M() << endl;
    }
  }
  if(newFormat){
    setWeight(array[counter++]);
    setGeneratorPdfRelativeToPhaseSpace(array[counter++]);
  }
  if(dbThis){
    cout << " weight, pdf "
	 << getWeight() << ", "
	 << getGeneratorPdfRelativeToPhaseSpace() << endl;
  }
  resetST();
  _perm.setPattern(_pat);
  if(dbThis){
    cout << "this is me now " << endl;
    this->print();
    cout << "done it all, returning" << endl;
  }
  return true;
}
bool DalitzEvent::fromParasTreeOld(TTree* ntp){
  bool dbThis=false;
  bool forcePDGMassForFinalState=true;

  if(dbThis) cout << "DalitzEvent::fromTREE(" << ntp << ") OLD called" << endl;
  
  int arraySize = ntp->GetNbranches();
  if(dbThis) cout << " arraySize " << arraySize << endl;
  bool newFormat = false;
  if(0 == (arraySize-2)%singleParticleNtpArraySize() ){
    newFormat = true; // includes weight & generatorPdf
    if(dbThis) cout << " new Format" << endl;
  }else if( 0 == (arraySize)%singleParticleNtpArraySize()){
    newFormat = false; // excludes weight & generatorPdf
    if(dbThis) cout << " old Format" << endl;
  }else{
    cout << "ERROR in DalitzEvent::fromTree length of array"
	 << " in ntuple = " << arraySize
	 << "\n   >  not a multiple of # entries per particle = "
	 << singleParticleNtpArraySize()
	 << endl;
    return false;
  }
  const TObjArray* branchArray = ntp->GetListOfBranches();

  if(dbThis) cout << " array ptr " << branchArray << endl;
  int counter = 0;
  int maxCounter = arraySize - (singleParticleNtpArraySize()-1);
  if(newFormat) maxCounter -= 2;
  int nParticles;
  if(newFormat){
    nParticles = (arraySize-2)/singleParticleNtpArraySize();
  }else{
    nParticles = arraySize/singleParticleNtpArraySize();
  }
  if(dbThis){
    cout << "nParticles = " << nParticles
	 << " arraySize = " << arraySize
	 << endl;
  }

  _pat.resize(nParticles);
  _p.resize(nParticles);
  for(int i=0; i< nParticles && counter < maxCounter; i++){
    if(dbThis) cout << "filling " << i << " th particle: " << endl;
    TObjArray* leafArray;
    double leafVal = -9999;
    std::string leafName;
    leafArray=
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    leafName = ((TLeaf*) (*leafArray)[0])->GetName();
    if(dbThis) cout << " leaf[ " << counter-1 << "] " 
		    << leafVal << endl;
    _p[i].SetE(leafVal*GeV);
    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    leafName = ((TLeaf*) (*leafArray)[0])->GetName();
    if(dbThis){
      cout << " leaf[ " << counter-1 << "] " << leafName 
	   << ": " << leafVal << endl;
    }
    _p[i].SetX(leafVal*GeV);
    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    if(dbThis) cout << " leaf[ " << counter-1 << "] " << leafVal << endl;
    _p[i].SetY(leafVal*GeV);
    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    if(dbThis) cout << " leaf[ " << counter-1 << "] " << leafVal << endl;
    _p[i].SetZ(leafVal*GeV);
    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    if(dbThis) cout << " leaf[ " << counter-1 << "] " << leafVal << endl;
    _pat[i] = nearestInt(leafVal);

    if(forcePDGMassForFinalState && i > 0){
      double m=_pat[i].mass();
      _p[i].SetE(sqrt(m*m + _p[i].Rho()*_p[i].Rho()));
    }
    if(dbThis) cout << " got pattern " << (int) _pat[i] << endl;

    if(dbThis){
      cout << "\t" << _pat[i] << " " << _p[i] 
	   << ", m= " << _p[i].M() << endl;
    }
  }
  if(newFormat){
    TObjArray* leafArray;
    double leafVal = -9999;
    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    if(dbThis) cout << " leaf[ " << counter-1 << "] " << leafVal << endl;
    setWeight(leafVal);

    leafArray = 
      ((TBranch*) (*branchArray)[counter++])->GetListOfLeaves();
    leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    setGeneratorPdfRelativeToPhaseSpace(leafVal);
  }
  if(dbThis){
    cout << " weight, pdf "
	 << getWeight() << ", "
	 << getGeneratorPdfRelativeToPhaseSpace() << endl;
  }
  resetST();
  _perm.setPattern(_pat);
  if(dbThis) cout << "done it all, returning" << endl;
  return true;
}

bool DalitzEvent::parseNtpEntryName(const std::string& entry
				    , std::string& part1
				    , std::string& part2){
  int posOf_ = entry.find_last_of('_');
  part1 = entry.substr(0, posOf_);
  part2 = entry.substr(posOf_ +1);
  return true;
}

bool DalitzEvent::fromParasTree(TTree* ntp){
  bool dbThis=false;
  bool forcePDGMassForFinalState=true;

  if(dbThis) cout << "DalitzEvent::fromTREE(" << ntp << ") called" << endl;
  
  int arraySize = ntp->GetNbranches();
  if(dbThis) cout << " arraySize " << arraySize << endl;
  const TObjArray* branchArray = ntp->GetListOfBranches();

  map<string, map<std::string, double> > prtMap;

  double weight=1;
  double genPdf=1;
  for(int i=0; i< arraySize; i++){
    TObjArray* leafArray=
      ((TBranch*) (*branchArray)[i])->GetListOfLeaves();
    double leafVal = ((TLeaf*) (*leafArray)[0])->GetValue();
    std::string leafName = ((TLeaf*) (*leafArray)[0])->GetName();
    
    std::string parName, element;
    parseNtpEntryName(leafName, parName, element);
    if(dbThis){
      cout << " leaf[ " << i << "] " 
	   << leafName << "; " << leafVal << endl;
    }

    if("weight" == leafName) weight=leafVal;
    else if("genPdf" == leafName) genPdf = leafVal;
    else{
      std::string parName, element;
      parseNtpEntryName(leafName, parName, element);
      (prtMap[parName])[element] = leafVal;
      if(dbThis){
	cout << "\t leaf[ " << i << "] name parsed: " 
	     << parName << ", " << element
	     << endl;
      }
    }    
  }
  int nParticles = prtMap.size();
  if(dbThis) cout << "nParticles = " << nParticles << endl;
  _pat.resize(nParticles);
  _p.resize(nParticles);
  
  int counter=0;
  for(map<string, map<string, double> >::iterator it = 
	prtMap.begin();
      it != prtMap.end();
      it++, counter++){
    _pat[counter] = (it->second)["pdg"];
    if(dbThis){
      cout << "just filled _pat[" << counter << "] with: "
	   << (it->second)["pdg"] << "; see: " << _pat[counter]
	   << endl;
    }
    _p[counter].SetE((it->second)["E"]);
    _p[counter].SetPx((it->second)["Px"]);
    _p[counter].SetPy((it->second)["Py"]);
    _p[counter].SetPz((it->second)["Pz"]);

    // check units (Paras usually GeV, else usually MeV)
    double units;
    double dMeV = abs(_pat[counter].mass() - _p[counter].M()*MeV);
    double dGeV = abs(_pat[counter].mass() - _p[counter].M()*GeV);
    if(dMeV < dGeV) units=MeV;
    else units=GeV;
    _p[counter] *= units;
    
    if(forcePDGMassForFinalState && counter > 0){
      double m  = _pat[counter].mass();
      double p3 = _p[counter].Rho();
      _p[counter].SetE(sqrt(m*m + p3*p3));
    }
  }
  setWeight(weight);
  setGeneratorPdfRelativeToPhaseSpace(genPdf);
  if(dbThis){
    cout << " weight, pdf "
	 << getWeight() << ", "
	 << getGeneratorPdfRelativeToPhaseSpace() << endl;
    for(int i=0; i < nParticles; i++){
      cout << _pat[i] << " ) " << _p[i] << " m = " << _p[i].M() << endl;
    }
  }
  resetST();
  _perm.setPattern(_pat);
  if(dbThis) cout << "done it all, returning" << endl;
  return true;
  }

// non-members:

std::ostream& operator<<(std::ostream& os, const DalitzEvent& de){
  de.print(os);
  return os;
}

//

