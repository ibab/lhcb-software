// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include <cmath>

#include "Mint/Utils.h"
#include "Mint/BW_BW.h"
#include "Mint/ParticleProperties.h"
//#include "fitSetup.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/BWFct.h"
#include "Mint/FlatFct.h"

using namespace std;
using namespace MINT;

bool bigDbg=false;

//bool PDGWithReco=false;
bool PDGWithReco=true;

bool DoAsLaurenDid = false;
bool compareToOldRooFit = DoAsLaurenDid;

BW_BW::BW_BW( const AssociatedDecayTree& decay)
  : _eventPtr(0)
  , _prSq(-9999.0)
  , _prSqForGofM(-9999.0)
  , _pABSq(-9999.0)
    //  , _mumsPDGMass(-9999.0)
    //  , _mumsPDGWidth(-9999.0) 
  , _mumsRecoMass2(-9999.0)
  , _mumsRecoMass(-9999.0)
  , _Fr_BELLE(-9999.0)
  , _Fr_PDG_BL(-9999.0)
  , _GofM(-9999.0)
  , _mumsPID(0)
  , _mumsPID_set(false)
    //  , _mumsPDGRadius(-9999.0)
  , _mumsParity(0)
  , _dgtrsInternalParity(0)
  , _twoLPlusOne(-9999)
  , _daughterRecoMass2(2, -9999.0)
  , _daughterPDGMass(2, -9999.0)
  , _daughterWidth(2, -9999.0)
  , _substitutePDGForReco(false)
  , _genFct(0)
  , _theDecay(decay)
  , _gen_s_mi(0)
  , _gen_s_ma(0)
  , _RPL(0)
  , _fittableResonancePropertiesPtr(0)
  , _fittableGlobalRadiusPtr(0)
  , _normBF("NormBF", 0)
{
  //  const fitSetup* fs = fitSetup::getMe();
  resetPDG();
  
  if(_theDecay.nDgtr() > 2 && ! (startOfDecayChain())){
    std::cout << "WARNING: BW_BW can only properly handle"
	      << "\n    > two body decays."
	      << "\n    > This: " << _theDecay.oneLiner()
	      << "\n    > is a " << _theDecay.nDgtr() << " body decay."
	      << "\n    > Please improve me."
	      << std::endl;
    std::cout << " startOfDecayChain()? " << startOfDecayChain()
	      << "  PID " << mumsPID()
	      << std::endl;
  }
  /*    
    _RPL = ResonancePropertiesList::getMe();
    if(_RPL->get(mumsPID())==0){
        ResonanceProperties* rp= new ResonanceProperties(mumsProperties()->name());
        _RPL->AddToList(rp);
    }  
  */
  setAllFitParameters();
}
BW_BW::BW_BW(const BW_BW& other)
  : ILineshape()
  , _prSq(other._prSq)
  , _prSqForGofM(other._prSqForGofM)
  , _pABSq(other._pABSq)
    //  , _mumsPDGMass(other._mumsPDGMass)
    //  , _mumsPDGWidth(other._mumsPDGWidth) 
  , _mumsRecoMass2(other._mumsRecoMass2)
  , _mumsRecoMass(other._mumsRecoMass)
  , _Fr_BELLE(other._Fr_BELLE)
  , _Fr_PDG_BL(other._Fr_PDG_BL)
  , _GofM(other._GofM)
  , _mumsPID(other._mumsPID)
  , _mumsPID_set(other._mumsPID_set)
    //  , _mumsPDGRadius(other._mumsPDGRadius)
  , _mumsParity(other._mumsParity)
  , _dgtrsInternalParity(other._dgtrsInternalParity)
  , _twoLPlusOne(other._twoLPlusOne)
  , _daughterRecoMass2(other._daughterRecoMass2)
  , _daughterPDGMass(other._daughterPDGMass)
  , _daughterWidth(other._daughterWidth)
  , _substitutePDGForReco(other._substitutePDGForReco)
  , _genFct(other._genFct)
  , _theDecay(other._theDecay)
  , _gen_s_mi(other._gen_s_mi)
  , _gen_s_ma(other._gen_s_ma)
  , _RPL(other._RPL)
  , _fittableResonancePropertiesPtr(0)
  , _fittableGlobalRadiusPtr(0)
  , _normBF(other._normBF)
{
  
  /*  if(_RPL->get(mumsPID())==0){
        ResonanceProperties* rp= new ResonanceProperties(mumsProperties()->name());
        _RPL->AddToList(rp);
    }  
  */
  setAllFitParameters();
}

BW_BW::~BW_BW(){
  delete _fittableResonancePropertiesPtr;
  delete  _fittableGlobalRadiusPtr;
}

bool BW_BW::setEventPtr(IDalitzEvent& evt) const{
  //_oldPointers.push(getEvent());
  _eventPtr = &(evt);
  return true;
}

IDalitzEvent* BW_BW::getEvent() const{
  return _eventPtr;
}

int BW_BW::twoLPlusOne() const{
  if( _twoLPlusOne < 0 ){
    int externallySetL = _theDecay.getVal().L();
    if( externallySetL >= 0){
      _twoLPlusOne = 2* externallySetL + 1;
    }else{
      _twoLPlusOne = lowestPossibleTwoLPlusOne();
    }
  }
  return _twoLPlusOne;
}

int BW_BW::lowestPossibleTwoLPlusOne()const{
  bool dbThis=false;
  int l_1 = abs(mumsSpinValue() - maxDaughterSpinSum());
  int l_2 = abs(mumsSpinValue() - minDaughterSpinSum());

  int l = (l_1 < l_2 ? l_1 : l_2);
  int maxL = mumsSpinValue() + maxDaughterSpinSum();

  if(dbThis){
    cout << "BW_BW::lowestPossibleTwoLPlusOne() "
	 << _theDecay.oneLiner()
	 << " before considering parity: " << 2*l+1
	 << " for " << mumsSpinValue() 
	 << (mumsParity() > 0 ? "+" : "-")
	 << " -> " 
	 << daughterSpinValue(0)
	 << (daughterP(0) > 0 ? "+" : "-")
	 << ", "
	 << daughterSpinValue(1)
	 << (daughterP(1) > 0 ? "+" : "-")
	 << " l = " << l
	 << " and maxL " << maxL
	 << " is weak decay? " << isWeakDecay()
	 << endl;
  }

  if(! isWeakDecay()){
    while((! parityConservingL(l)) && l < maxL) l++;
  }
  if(dbThis){
    if(isWeakDecay()){
      cout << " is weak decay: mum " << mumsQuarkContent()
	   << " daughters: " << dgtrsQuarkContent() << endl;
      cout << "is weak decay - would it have made a difference?"
	   << " now l = " << l;
      int tmpL=l;
      while((! parityConservingL(tmpL)) && tmpL < maxL) tmpL++;
      cout << ", would have been: " << tmpL << endl;
    }
    cout << "\tBW_BW::lowestPossibleTwoLPlusOne() "
	 << " returning " << 2*l+1
	 << " (l=" << l <<")"
	 << endl;
  }

  return 2*l + 1;
}

bool BW_BW::isWeakDecay() const{
  return ! mumsQuarkContent().compatible(dgtrsQuarkContent());
}

MultiQuarkContent BW_BW::mumsQuarkContent() const{
  return netQuarkContent(_theDecay); // handles non-res

  //return mumsProperties()->netQuarkContent();
}

bool BW_BW::nonResonant() const{
  return mumsProperties()->isNonResonant();
}

MultiQuarkContent BW_BW::dgtrsQuarkContent() const{
  MultiQuarkContent sum;
  for(int i=0; i < numDaughters(); i++){
    sum += daughterQuarkContent(i);
  }
  return sum;
}

bool BW_BW::parityConservingL(int L) const{
  int PL = (L%2 == 0 ? 1 : -1);
  return ((PL * dgtrsInternalParity()) == mumsParity());
}

int BW_BW::mumsParity() const{
  if( 0 == _mumsParity ){
    const ParticleProperties* pp = _theDecay.getVal().props();
    if(0==pp){
      cout << "ERROR in BW_BW::mumsParity()"
	   << " can't find properties for first element"
	   << " in this decay tree\n" << _theDecay << endl;
      throw "invalid decay tree in BW_BW::mumsParity()";
    }
    _mumsParity = pp->P();
  }
  return _mumsParity;
}

int BW_BW::numDaughters() const{
  return _theDecay.nDgtr();
}
int BW_BW::daughterP(int i) const{
  if( i >= numDaughters() || i < 0){
    cout << " ERROR in BW_BW::daughterSpinValue:"
	 << " You requested the spin of dgtr number " << i
	 << ". There are " << numDaughters()
	 << " daughters." << endl;
    return 0;
  }
  const ParticleProperties* pp = _theDecay.getDgtrVal(i).props();
  if(0==pp){
    cout << "ERROR in BW_BW::daughterP(" << i <<")"
	 << " can't find properties for first element"
	 << " in this decay tree\n" << _theDecay << endl;
    throw "invalid decay tree in BW_BW::daughterP()";
  }
  return pp->P();
}

int BW_BW::dgtrsInternalParity() const{
  if( 0 == _dgtrsInternalParity ){
    _dgtrsInternalParity = 1;
    for(int i=0; i < numDaughters(); i++){
      _dgtrsInternalParity *= daughterP(i);
    }
  }
  return _dgtrsInternalParity;

}

int BW_BW::maxDaughterSpinSum()const{
  int sum=0;
  for(int i=0; i < numDaughters(); i++){
    sum += daughterSpinValue(i);
  }
  return sum;
}

int BW_BW::minDaughterSpinSum() const{
  if(2 == numDaughters()){
    return minDaughterSpinSum2();
  }else if(3 == numDaughters()){
    return minDaughterSpinSum3();
  }if(4 == numDaughters()){
    return minDaughterSpinSum4();
  }else{
    cout << "BW_BW::minDaughterSpinSum() WARNING! "
	 << " can't handle more than 4 daughters - have "
	 << numDaughters() << ". Please improve me." << endl;
    return 0;
  }
}

/*
  generic spin values, no restriction on num daughters
   - not really needed, implement another day 
int BW_BW::maxConseqDaughterSpinSum(int minIndex, int maxIndex) const{
  if(maxIndex < minIndex) return 0;
  int sum=0;
  for(int i=minIndex; i <= maxIndex; i++){
    sum += daughterSpinValue(i);
  }
  return sum;
}

int BW_BW::minConseqDaughterSpinSum(int minIndex, int maxIndex) const{
  if(minIndex == maxIndex){
    return daughterSpinValue(minIndex);
  }else if(minIndex + 1 == maxIndex){
    return minDaughterPairSpinSum(minIndex, maxIndex);
  }

  int minSpinMany = minConseqDaughterSpinSum(minIndex + 1, maxIndex);
  int minSpinMany = maxConseqDaughterSpinSum(minIndex + 1, maxIndex);
  int singleSpin = daughterSpinValue(minIndex);

  if(minSpinMany <=  singleSpin && maxSpinMany >= singleSpin){
    return 0;
  }else if (minSpinMany > singleSpin){
    return abs(minSpinMany - singleSpin);
  }else{
    return abs(maxSpinMany - singleSpin);
  }
}
*/

int BW_BW::minDaughterSpinSum2() const{
  return abs(daughterSpinValue(1) - daughterSpinValue(0));
}
int BW_BW::minDaughterSpinSum3() const{
  int minSpinPair = minDaughterPairSpinSum(0,1);
  int maxSpinPair = maxDaughterPairSpinSum(0,1);
  int otherSpin = daughterSpinValue(2);
  int min = maxSpinPair + otherSpin;
  for(int s = minSpinPair; s <= maxSpinPair; s++){
    int thisMinSpin = abs(s - otherSpin);
    if(thisMinSpin < min) min=thisMinSpin;
  }
  return min;
}
int BW_BW::minDaughterSpinSum4() const{
  int minSpinPair1 = minDaughterPairSpinSum(0,1);
  int maxSpinPair1 = maxDaughterPairSpinSum(0,1);
  
  int minSpinPair2 = minDaughterPairSpinSum(2,3);
  int maxSpinPair2 = maxDaughterPairSpinSum(2,3);
  
  int min = maxSpinPair1 + maxSpinPair2;
  for(int s1 = minSpinPair1; s1 <= maxSpinPair1; s1++){
    for(int s2 = minSpinPair2; s2 <= maxSpinPair2; s2++){
      int thisMinSpin = abs(s1 - s2);
      if(thisMinSpin < min) min=thisMinSpin;
    }
  }
  return min;
}

int BW_BW::maxDaughterPairSpinSum(int i, int j)const{
  return daughterSpinValue(i) + daughterSpinValue(j);
}

int BW_BW::minDaughterPairSpinSum(int i, int j) const{
  return abs(daughterSpinValue(i) - daughterSpinValue(j));
}


const ParticleProperties* BW_BW::mumsProperties() const{
  const ParticleProperties* pp = _theDecay.getVal().props();
  if(0==pp){
    cout << "ERROR in BW_BW::mumsProperties()"
	 << " can't find properties for first element"
	 << " in this decay tree\n" << _theDecay << endl;
    throw "invalid decay tree in BW_BW::mumsProperties()";
  }
  return pp;
}

ResonancePropertiesList*  BW_BW::resonancePropertiesList() const{
  if(0 == _RPL) _RPL = ResonancePropertiesList::getMe();
  if(0 == _RPL){
    cout << "ERROR in BW_BW::resonanceProperties()"
	 << " can't find properties for first element"
	 << " in this decay tree\n" << _theDecay << endl;
    throw "invalid decay tree in BW_BW::resonanceProperties()";
  }
  return _RPL;
}

const ResonanceProperties* BW_BW::resonanceProperties() const{
  const ResonanceProperties* rp = resonancePropertiesList()->get(mumsPID());
  if(0 == rp){
    ResonanceProperties* rpnew= new ResonanceProperties(mumsProperties()->pdg_id());
    resonancePropertiesList()->AddToList(rpnew);
    rp=rpnew;
  }
  if(0 == rp){
    cout << "ERROR in BW_BW::ResonanceProperties()"
	 << " can't find properties for first element"
	 << " in this decay tree\n" << _theDecay << endl;
    throw "invalid decay tree in BW_BW::mumsFittableProperties()";
  }
  return rp;
}

ResonancePropertiesFitRef& BW_BW::mumsFittableProperties() const{
  if(0 == _fittableResonancePropertiesPtr){
    cout << "something went wrong in BW_BW::mumsFittableProperties() "
	 << " _fittableResonancePropertiesPtr is 0 although it should be"
	 << " set at construction."
	 << " Looking at  this decay tree\n" << _theDecay << endl;

    throw "error in BW_BW::mumsFittableProperties()";
  }
  return *_fittableResonancePropertiesPtr;
}

bool BW_BW::setAllFitParameters(){
  bool s=true;
  if(0 == resonancePropertiesList()){
    cout << "big problem in BW_BW::setAllFitParameters"
	 << ", resonancePropertiesList is zero" << endl;
    throw "BW_BW::setAllFitParameters can't find resonancePropertiesList()";
  }
  cout << "getting _fittableGlobalRadiusPtr" << endl;
  const FitParameter& rad(resonancePropertiesList()->radius());
  cout << "called radius, no crash, now pringint it:" << endl;
  cout << "rad is" << rad << endl;
  cout << " radius is: " << resonancePropertiesList()->radius() << endl;
  _fittableGlobalRadiusPtr = new FitParRef(resonancePropertiesList()->radius(), this);
  cout << "hurray, got _fittableGlobalRadiusPtr" << endl;
  s &= (0 != _fittableGlobalRadiusPtr);
  _fittableResonancePropertiesPtr = new ResonancePropertiesFitRef(*resonanceProperties(), this);
  s &= (0 != _fittableResonancePropertiesPtr);
  return s;
}


double BW_BW::mumsMass() const{
  bool dbThis=false;
  return mumsFittableProperties().mass();
  // return _RPL->get(mumsPID())->mass();
}

double BW_BW::mumsWidth() const{
  return mumsFittableProperties().width();
  //    return _RPL->get(mumsPID())->width();
}

double BW_BW::mumsRadius() const{
  return mumsFittableProperties().radius();
  //    return _RPL->get(mumsPID())->radius();
}

double BW_BW::Radius() const{
  if(0 == _fittableGlobalRadiusPtr){
    cout << "ERROR in BW_BW::Radius()"
	 << " _fittableGlobalRadiusPtr is 0"
	 << ", although it should have been set during construction."
	 << " Looking at  this decay tree\n" << _theDecay << endl;
    throw "invalid decay tree in BW_BW::Radius()";
  }
  return *_fittableGlobalRadiusPtr;
}

/*
double BW_BW::mumsPDGMass() const{
  if(_mumsPDGMass < 0){
    _mumsPDGMass = mumsProperties()->mass();
  }
  return _mumsPDGMass;
}

double BW_BW::mumsPDGWidth() const{
  if(_mumsPDGWidth < 0){
    _mumsPDGWidth = mumsProperties()->width();
  }
  return _mumsPDGWidth;
}

double BW_BW::mumsPDGRadius() const{
    if(_mumsPDGRadius < -9998){
        _mumsPDGRadius = mumsProperties()->radius();
    }
    return _mumsPDGRadius;
}
*/

std::string BW_BW::mumsSpin() const{
  return mumsProperties()->J();
}

int BW_BW::mumsSpinValue() const{
  return atoi(mumsSpin().c_str());
}

int BW_BW::mumsPID() const{
  if(! _mumsPID_set){
    _mumsPID = mumsProperties()->pdg_id();
    _mumsPID_set = true;
  }
  return _mumsPID;
}
/*
bool BW_BW::thisIsD() const{
  return (abs(mumsPID()) == 421 
	  || abs(mumsPID()) == 411 
	  || abs(mumsPID()) == 431);
}
*/

bool BW_BW::startOfDecayChain() const{
  return ! (_theDecay.hasParent());
}


DalitzCoordinate BW_BW::getDalitzCoordinate(double nSigma) const{
  DalitzCoordinate coord(_theDecay.getVal().asi());
  double meanM = mumsMass();
  double width = mumsWidth();
  
  double minMass = meanM - nSigma*width/2.0;
  double maxMass = meanM + nSigma*width/2.0;

  coord.setMinMax(minMass*minMass, maxMass*maxMass);
  return coord;
}
double BW_BW::mumsRecoMass2() const{
  if(substitutePDGForReco()) return mumsMass()*mumsMass();

  if(_mumsRecoMass2 < 0){
    std::vector<int> asi = _theDecay.getVal().asi();
    if(asi.size() < 2){
      cout << "ERROR in BW_BW::mumsRecoMass2() "
	   << _theDecay.getVal()._pdg_id
	   << ", " << _theDecay.getVal().name()
	   << "  decays into " << asi.size() << " particles?\n" 
	   << _theDecay
	   << endl;
    }
    _mumsRecoMass2 = getEvent()->sij(asi);
  }
  return _mumsRecoMass2;
}

TLorentzVector BW_BW::daughterP4(int i) const{
  // for diagnostics only.
  if( i >= _theDecay.nDgtr() || i < 0){
    cout << " ERROR in BW_BW::daughterP4:"
	 << " You requested the 4-momentum of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr() 
	 << " daughters." << endl;
    exit(1);
  }
  const_counted_ptr<AssociatedDecayTree> dgtr = _theDecay.getDgtrTreePtr(i);
  std::vector<int> asi = dgtr->getVal().asi();
  if(asi.size() < 2){
    return getEvent()->p(asi[0]);
  }else{
    return TLorentzVector();
  }
}
double BW_BW::daughterRecoMass2(int i) const{
  if(substitutePDGForReco()) return daughterPDGMass(i)*daughterPDGMass(i);

  if( i >= _theDecay.nDgtr() || i < 0){
    cout << " ERROR in BW_BW::daughterRecoMass2:"
	 << " You requested the mass of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr() 
	 << " daughters." << endl;
    return -9999;
  }
  if(_daughterRecoMass2[i] < 0){
    
    const_counted_ptr<AssociatedDecayTree> dgtr = _theDecay.getDgtrTreePtr(i);
    if(dgtr->isFinalState()){
      double m = daughterPDGMass(i);
      _daughterRecoMass2[i] = m*m;
      //_daughterRecoMass2[i] = daughterP4(i).M2();
    }else{
      std::vector<int> asi = dgtr->getVal().asi();
      if(asi.size() < 2){
	cout << "ERROR in BW_BW::daughterRecoMass2() "
	     << dgtr->getVal()._pdg_id
	     << ", " << dgtr->getVal().name()
	     << " is associated to " << asi.size() << " particles"
	     << " and is not stable? "
	     << _theDecay
	     << endl;
	
      } 
      _daughterRecoMass2[i] = getEvent()->sij(asi);
    }
  }
  return _daughterRecoMass2[i];
}
double BW_BW::daughterRecoMass(int i) const{
  if(substitutePDGForReco()) return daughterPDGMass(i);

  double m2 = daughterRecoMass2(i);
  if(m2 < 0){
    cout << " ERROR negative reconstructed mass2"
	 << " in decay:\n " << _theDecay << endl;
    return - sqrt(-m2);
  }
  //  cout << " for index " << i << endl;
  //  cout << " m2 = " << m2 << " mass = " << sqrt(m2) << endl;
  return sqrt(m2);
}

double BW_BW::mumsRecoMass() const{
  if(substitutePDGForReco()){
    return mumsMass();
  }
  if(_mumsRecoMass < 0){
    double m2 = mumsRecoMass2();
    if(m2 < 0){
      cout << " ERROR negative reconstructed mass2"
	   << " in decay:\n " << _theDecay << endl;
      _mumsRecoMass = - sqrt(-m2);
    }else{
      _mumsRecoMass = sqrt(m2);
    }
  }
  return _mumsRecoMass;
}

double BW_BW::daughterPDGMass( const int& i ) const{
  if( i >= _theDecay.nDgtr() || i < 0 ){
    cout << " ERROR in BW_BW::daughterPDGMass:"
	 << " You requested the mass of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr()
	 << " daughters." << endl;
    return -9999;
  }

  if( _daughterPDGMass[i] < 0 ){
    _daughterPDGMass[i] = _theDecay.getDgtrVal(i).mass();
  }

  if( _theDecay.getDgtrVal(i).isNonResonant() ){
    _daughterPDGMass[i] = daughterRecoMass(i);
  }

  return _daughterPDGMass[i];
}

double BW_BW::daughterWidth(int i) const{
  if( i >= _theDecay.nDgtr() || i < 0){
    cout << " ERROR in BW_BW::daughterWidth:"
	 << " You requested the mass of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr() 
	 << " daughters." << endl;
    return -9999;
  }
  if(_daughterWidth[i] < 0){
    _daughterWidth[i] =  _theDecay.getDgtrVal(i).width();
  }
  return _daughterWidth[i];
}

std::string BW_BW::daughterSpin(int i) const{
  if( i >= _theDecay.nDgtr() || i < 0){
    cout << " ERROR in BW_BW::daughterSpinValue:"
	 << " You requested the spin of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr() 
	 << " daughters." << endl;
    return "-9999";
  }
  return _theDecay.getDgtrVal(i).J();
}
MultiQuarkContent BW_BW::daughterQuarkContent(int i) const{
  if( i >= _theDecay.nDgtr() || i < 0){
    cout << " ERROR in BW_BW::daughterQuarkContent"
	 << " You requested the spin of dgtr number " << i
	 << ". There are " << _theDecay.nDgtr() 
	 << " daughters." 
	 << "\n\t I'll crash now. Sorry."
	 << endl;
    throw "can't handle this";
  }

  return netQuarkContent(*(_theDecay.getDgtrTreePtr(i)));
  //return _theDecay.getDgtrVal(i).props()->netQuarkContent();
}

int BW_BW::daughterSpinValue(int i) const{
  return atoi(daughterSpin(i).c_str());
}
double BW_BW::Fr(){
  return Fr_BELLE(prSq());
  // return Fr_PDG_BL();
}
double BW_BW::FrForGofM(){
  return Fr_BELLE(prSqForGofM());
}
double BW_BW::FrMax(){
  return Fr_BELLE_Max();
}

double BW_BW::Fr_BELLE(double prSquared){
  // (ratio of) Blatt-Weisskopf penetration factors
  // as in BELLE note hep-ex/0406067, pg 5.
  bool dbThis=false;
  //if(startOfDecayChain()) dbThis=true;

  if(dbThis){
    cout << " mums name " << mumsProperties()->name() << endl;
    cout << " decay chain " << _theDecay.oneLiner() << endl;
    cout << " twoLPlusOne: " << twoLPlusOne() << endl;
  }

  if(_Fr_BELLE < 0){
    if(twoLPlusOne() == 1){                        // spin = 0
      _Fr_BELLE = 1;
      if(dbThis){
	cout << " _Fr_BELLE = " << _Fr_BELLE << endl;
      }
      return _Fr_BELLE;
    }
    double R = Radius(); // or mumsRadius()  ???
    double R_pr_sq  = R*R * prSquared;
    double R_pAB_sq = R*R * pABSq();
    if(twoLPlusOne() == 3){                        // spin = 1
      if(dbThis) cout << "in Fr() " << sqrt(prSquared)
		      << ", " << sqrt(pABSq()) << endl;
      
      if(dbThis)cout << "BW Fr R = " << R << "prSquared = " << prSquared 
		     << " pABSq() " << pABSq()
		     << endl;
      _Fr_BELLE = sqrt( (1. + R_pr_sq)/(1. + R_pAB_sq) );
    }else if(twoLPlusOne() == 5){                   // spin == 2
      double R_pr_4  = R_pr_sq*R_pr_sq;
      double R_pAB_4 = R_pAB_sq*R_pAB_sq;
      
      _Fr_BELLE = sqrt( (  9. + 3.*R_pr_sq  + R_pr_4)
		  /( 9. + 3.*R_pAB_sq + R_pAB_4) 
		  );
    } else{
      std::cout << "BW_BW for decay \n" << _theDecay 
		<< "\n ERROR! Can't deal with L == " 
		<< twoLPlusOne()
		<< ". Please improve me!"
		<< std::endl;
      _Fr_BELLE = -9999;
    }
  }
  if(dbThis){
    cout << "returning _Fr_BELLE = " << _Fr_BELLE << endl;
  }
  return _Fr_BELLE;
}
double BW_BW::Fr_BELLE_Max(){
  // Max value of Blatt-Weisskopf penetration factors
  // as in BELLE note hep-ex/0406067, pg 5.
  bool dbThis=false;
  //if(startOfDecayChain()) dbThis=true;

  if(dbThis){
    cout << " mums name " << mumsProperties()->name() << endl;
    cout << " decay chain " << _theDecay.oneLiner() << endl;
    cout << " twoLPlusOne: " << twoLPlusOne() << endl;
  }

  double FrBelleMax=-9999;

  if(twoLPlusOne() == 1){                        // spin = 0
    FrBelleMax = 1;
    if(dbThis){
      cout << " FrBelleMax = " << FrBelleMax << endl;
    }
    return FrBelleMax;
  }
  double R = Radius(); // or mumsRadius()  ???
  //  double R_pr_sq  = R*R * prSq();
  double R_pr_sq  = R*R * prSq();
  double R_pAB_sq = 0;//R*R * pABSq();
  if(twoLPlusOne() == 3){                        // spin = 1
    if(dbThis) cout << "in FrMax() " << sqrt(prSq())
		    << ", " << sqrt(pABSq()) << endl;
    
    if(dbThis)cout << "BW FrMax R = " << R << "prSq() = " << prSq() 
		   << " pABSq() " << pABSq()
		   << endl;
    FrBelleMax = sqrt( (1. + R_pr_sq)/(1. + R_pAB_sq) );
  }else if(twoLPlusOne() == 5){                   // spin == 2
    double R_pr_4  = R_pr_sq*R_pr_sq;
    double R_pAB_4 = 0;//R_pAB_sq*R_pAB_sq;
    
    FrBelleMax = sqrt( (  9. + 3.*R_pr_sq  + R_pr_4)
		       /( 9. + 3.*R_pAB_sq + R_pAB_4) 
		       );
  } else{
    std::cout << "BW_BW for decay \n" << _theDecay 
	      << "\n ERROR! Can't deal with L == " 
	      << twoLPlusOne()
	      << ". Please improve me!"
	      << std::endl;
    FrBelleMax = -9999;
  }
  if(dbThis){
    cout << "returning FrBelleMax = " << FrBelleMax << endl;
    cout << " compare: Fr: " << Fr() << endl;
  }
  return FrBelleMax;
}
double BW_BW::Fr_PDG_BL(){
  // (ratio of) Blatt-Weisskopf penetration factors
  // as David Asner's PDG review, Table 1 (page 3),
  // column 2 (BELLE, above is col 3 in the same table)

  bool dbThis = false;
  //if(startOfDecayChain()) dbThis=true;

  if(dbThis){
    cout << " BW_BW::Fr mums name " << mumsProperties()->name() << endl;
    cout << " decay chain " << _theDecay.oneLiner() << endl;
    cout << " twoLPlusOne: " << twoLPlusOne() << endl;
  }

  if(_Fr_PDG_BL < 0){
    if(twoLPlusOne() == 1){                        // spin = 0
      _Fr_PDG_BL = 1;
      if(dbThis){
	cout << " _Fr_PDG_BL = " << _Fr_PDG_BL << endl;
      }
      return _Fr_PDG_BL;
    }
    double R = Radius(); // or mumsRadius()  ???
    //    double R_pr_sq  = R*R * prSq();
    double R_pAB_sq = R*R * pABSq();

    double z = R_pAB_sq;

    double FrSq = -9999;

    if(twoLPlusOne() == 3){                        // spin = 1
      if(dbThis) cout << "in Fr() " << sqrt(prSq())
		      << ", " << sqrt(pABSq()) << endl;
      
      if(dbThis)cout << "BW Fr R = " << R << "prSq() = " << prSq() 
		     << " pABSq() " << pABSq()
		     << endl;
      FrSq = 2.0*z/(1.0 + z);
      if(FrSq >=0) _Fr_PDG_BL = sqrt(FrSq);
      else _Fr_PDG_BL = 0;
    }else if(twoLPlusOne() == 5){                   // spin == 2
      FrSq = 13.0*z*z / ( (z-3.0)*(z-3.0) + 9*z );
      if(FrSq >=0) _Fr_PDG_BL = sqrt(FrSq);
      else _Fr_PDG_BL = 0;     
    }else{
      std::cout << "BW_BW for decay \n" << _theDecay 
		<< "\n ERROR! Can't deal with L == " 
		<< twoLPlusOne()
		<< ". Please improve me!"
		<< std::endl;
      _Fr_PDG_BL = -9999;
    }
  }
  if(dbThis){
    cout << "returning _Fr_PDG_BL = " << _Fr_PDG_BL << endl;
  }
  return _Fr_PDG_BL;
}

double BW_BW::twoBody_dgtPsq_in_MumsPDGFrame() const{
  if(! PDGWithReco){
    return twoBody_dgtPsq_in_MumsFrame(mumsMass()
				       , daughterPDGMass(0)
				       , daughterPDGMass(1)
				       );
  }else{
    return twoBody_dgtPsq_in_MumsFrame(mumsMass()
				       , daughterRecoMass(0)
				       , daughterRecoMass(1)
				       );
  }
}

double BW_BW::twoBody_recodgtPsq_in_MumsPDGFrame() const{
  return twoBody_dgtPsq_in_MumsFrame(mumsMass()
				     , daughterRecoMass(0)
				     , daughterRecoMass(1)
				     );
}

double BW_BW::twoBody_dgtPsq_in_MumsRecoFrame(){
  return twoBody_dgtPsq_in_MumsFrame(mumsRecoMass()
				     , daughterRecoMass(0)
				     , daughterRecoMass(1)
				     );
}

double BW_BW::twoBody_dgtPsq_in_MumsFrame(double mumsMass
					  , double mA
					  , double mB)const{
  bool dbThis=false;

  double Msq = mumsMass*mumsMass;
  if(Msq <=0) return -9999;
  
  double mpsq = (mA + mB)*(mA + mB);
  double mmsq = (mA - mB)*(mA - mB);
  double num = (Msq - mpsq)*(Msq - mmsq);
  
  double returnVal = num/(4*Msq);

  if(dbThis){
    double mAB = mumsMass;
    double EvtGenVal = (((mAB*mAB-mA*mA-mB*mB)*(mAB*mAB-mA*mA-mB*mB)/4.0) -
			mA*mA*mB*mB)/(mAB*mAB);
    cout << " BW_BW::twoBody_dgtPsq_in_MumsFrame"
	 << " compare: cleo: " << EvtGenVal
	 << " MINT: " << returnVal
	 << endl;
  }
  return returnVal;
  
}

double BW_BW::prSqMax() const{


  std::vector<const AssociatedDecayTreeItem*> dgtrOne = 
    _theDecay.getDgtrTreePtr(0)->finalState();
  double m1_min=0;
  for(unsigned int i=0; i < dgtrOne.size(); i++) m1_min += dgtrOne[i]->mass();

  std::vector<const AssociatedDecayTreeItem*> dgtrTwo = 
    _theDecay.getDgtrTreePtr(1)->finalState();
  double m2_min=0;
  for(unsigned int i=0; i < dgtrTwo.size(); i++) m2_min += dgtrTwo[i]->mass();

  /*
  double m1 = daughterPDGMass(0) - 0.5*daughterWidth(0);
  if(m1 < m1_min) m1 = m1_min;

  double m2 = daughterPDGMass(1) - 0.5*daughterWidth(1);
  if(m2 < m2_min) m2 = m2_min;
  */

  return twoBody_dgtPsq_in_MumsFrame(mumsMass(), m1_min, m2_min);

  //  return mumsPDGMass()*mumsPDGMass()/4.0;
}
double BW_BW::prSq() const{
  bool dbThis=false;
  if(_prSq <  0){
    _prSq = twoBody_dgtPsq_in_MumsPDGFrame();
    if(dbThis && _prSq < 0){ //dbg
      cout << " BW_BW::prSq()"
	   << " suspicious... pr < 0. This is for:"
	   << "\n   mumM PDG " << mumsMass()
	   << ", m1_nominal " << daughterPDGMass(0)
	   << ", m2_nominal " << daughterPDGMass(1)
	   << ", m1_reco " << daughterRecoMass(0)
	   << ", m2_reco " << daughterRecoMass(1)
	   << " _prSq " << _prSq
	   << " will return 0 "
	   << endl;
    }
  }
  if(_prSq < 0) _prSq = 0;
  return _prSq;
}
double BW_BW::prSqForGofM() const{
  bool dbThis=false;
  if(DoAsLaurenDid) return prSq();

  if(_prSqForGofM <  0){
    _prSqForGofM = fabs(twoBody_recodgtPsq_in_MumsPDGFrame()); // forces > 0.
  }
  if(dbThis || _prSqForGofM < 0){ //dbg
    cout << " compare : " << _prSqForGofM
	 << ", " << fabs(twoBody_recodgtPsq_in_MumsPDGFrame())
	 << endl;
    cout << " BW_BW::prSqForGofM()"
	 << " suspicious... pr < 0. This is for:"
	 << "\n   mumM PDG " << mumsMass()
	 << ", m1_nominal " << daughterPDGMass(0)
	 << ", m2_nominal " << daughterPDGMass(1)
	 << ", m1_reco " << daughterRecoMass(0)
	 << ", m2_reco " << daughterRecoMass(1)
	 << " _prSq " << _prSqForGofM
	 << " will return 0 "
	 << endl;
  }
  if(_prSqForGofM < 0) _prSqForGofM = 0; // can not happen as I'm using fabs above...
  return _prSqForGofM;
}
double BW_BW::pABSq(){
  bool dbThis=false;
  if(_pABSq < 0 || dbThis){
    _pABSq = twoBody_dgtPsq_in_MumsRecoFrame();
    if(_pABSq < 0 || dbThis){ //dbg
      cout << "In " << _theDecay.oneLiner()
	   << " BW_BW::pABSq()"
	   << " suspicious... pABSq < 0. This is for:"
	   << "\n   mumM = " << mumsRecoMass() 
	   << "\n, m1 " << daughterRecoMass(0)
	   << ", p1 " << daughterP4(0) << ", " << daughterP4(0).M()
	   << "\n , m2 " << daughterRecoMass(1)
	   << ", p2 " << daughterP4(1) << ", " << daughterP4(1).M()
	   << " returnVal " << _pABSq
	   << endl;
    }
    if(_pABSq < 0) _pABSq = 0;
  }
  return _pABSq;
}

double BW_BW::GofM(){
  //  return mumsWidth(); // debug only!
  bool dbThis=false;
  if(_GofM < 0){
    double densq = prSqForGofM();  
    double numsq = pABSq();
    if(numsq < 0){
      cout << "ERROR in BW_BW::GofM() for decay\n" 
	   << _theDecay
	   << "\n ERROR: pABSq() = " << pABSq() << endl;
      _GofM = -9999;
      return _GofM;
    }
    double epsilon = 1 * eV*eV;
    if(0.0 == densq ) densq = epsilon;
    
    double pratio = 1;
    if(densq > 0 && numsq >=0){
      pratio = sqrt(numsq/densq);
    }
    if(dbThis){
      cout << "pratio = sqrt( " << numsq << " / " << densq << " )" << endl;
    }
    double pratio_to_2Jplus1 = 1;
    if(dbThis) cout  << " pratio " 
			       << pratio 
			   //  << "^  2*mumsSpinValue() + 1 = "; //dbg
			       << "^ " << twoLPlusOne() << " = "; //dbg
    //    for(int i=0; i < 2*mumsSpinValue() + 1; i++){
    for(int i=0; i < twoLPlusOne(); i++){
      pratio_to_2Jplus1 *= pratio;
    }
    if(dbThis) cout << pratio_to_2Jplus1 << endl;//dbg
    
    densq = mumsRecoMass2();
    if(densq <= 0){
      cout << "ERROR in BW_BW::GofM() for decay\n"
	   << _theDecay
	   << ": ERROR: mumsRecoMass() = " 
	   << mumsRecoMass() << endl;
      _GofM = -9999;
      return _GofM;
    }
    double mRatio = mumsMass()/sqrt(densq);
    // this factor varies for different final states - check!!
    
    double thisFr = FrForGofM();
    if(dbThis) cout << "GofM() = " << mumsWidth()  << " * "  // dbg
			      << pratio_to_2Jplus1 << " * "
			      << mRatio << " * " 
			      << thisFr << " * " << thisFr
			      << endl;
    _GofM = mumsWidth() * pratio_to_2Jplus1 * mRatio * thisFr*thisFr;
    if(dbThis) cout << " so, GofM/G0 = " << _GofM/mumsWidth() << endl;
  }
  return _GofM;
}

std::complex<double> BW_BW::BreitWigner() {
  double mpdg = mumsMass();
  
  std::complex<double> invBW(mpdg*mpdg - mumsRecoMass2(), - mpdg * GofM());
  return 1.*GeV*GeV/invBW;
  //  return std::complex<Double_t>(ReBreitWigner(), ImBreitWigner());
}

void BW_BW::setGenerationLimits(double mi, double ma){
  _gen_s_mi = mi;
  _gen_s_ma = ma;
}

void BW_BW::resetInternals(){
  _prSq =  _prSqForGofM = _pABSq = _mumsRecoMass2 =  _mumsRecoMass = _Fr_BELLE = _Fr_PDG_BL =  _GofM = -1;

  _daughterRecoMass2.resize(_theDecay.nDgtr());
  for(int i=0; i< _theDecay.nDgtr(); i++){
    _daughterRecoMass2[i] = -1;
  }
}

void BW_BW::resetPDG(){
  //  _mumsPDGMass = _mumsPDGWidth = -1;
  _mumsPID_set = false;
  //  _mumsPDGRadius  = -9999.0;

  _daughterPDGMass.resize(_theDecay.nDgtr());
  for(int i=0; i< _theDecay.nDgtr(); i++){
    _daughterPDGMass[i] = -1;
  }
  _daughterWidth.resize(_theDecay.nDgtr());
  for(int i=0; i< _theDecay.nDgtr(); i++){
    _daughterWidth[i] = -1;
  }
}

std::complex<double> BW_BW::getVal(IDalitzEvent& evt){
  bool dbThis=bigDbg;

  setEventPtr(evt);
  resetInternals();

  double formFactor= 1.;
  if( _normBF == 1 ) formFactor = Fr();
  else if( _normBF == 0 ) formFactor = Fr_PDG_BL();  
  else if(_normBF == 3 ) formFactor = Fr_BELLE(0.);  
    
  if( nonResonant() ){
        return formFactor;
  }
    
  if(startOfDecayChain()){
    // in principle there is no need to distinguish the start
    // of the decay chain from the rest - it could just get
    // a Breit Wigner (with a constant value of the width of 
    // the D is zero, as usual). However, 
    // this is to comply with the usual convention: Only the
    // form factor, not the BW-propagator.
    if (compareToOldRooFit){
      return 1;
    }
    if(_theDecay.nDgtr() > 2){
      // all calculations of Fr etc are meaningless in this case
      // assume Fr=1;
      if(dbThis){
          cout << "BW_BW::getVal() for " << name() << endl;
          cout << "instead of Fr() for 2l+1 = " << twoLPlusOne() << endl;
          cout << " I'll return 1 " << endl;
      }
      return 1;
    }
    return formFactor;
  }
  
  if(dbThis){
    if(0 != getEvent()){
      cout << "BW pz " << getEvent()->p(1).Z() << endl;
    }
    cout << " BW_BW for " 
	 << _theDecay.oneLiner() << endl; // dbg
    cout << "\n    >  nominalMass " << mumsMass()
	 << "\n    > nominalWidth " << mumsWidth()
	 << "\n    > GoM() " << GofM()
	 << "\n    > Fr() " << Fr()
	 << "\n    > BW   " << BreitWigner()
	 << "\n    > FR*BW = " << Fr() * BreitWigner()
	 << "\n    > recoMass " << mumsRecoMass()
	 << "\n    > EvtGenValue " << EvtGenValue(*(getEvent()))
	 << endl;
  }
    
  const std::complex<double> returnVal = formFactor*BreitWigner();

  if(dbThis) cout << " value = " << returnVal 
       << "|A|^2 | " << returnVal.real()*returnVal.real() 
	       + returnVal.imag()*returnVal.imag()
       << endl; //dbg

  return returnVal;
}


double BW_BW::peakShift() const{
  // from: J.D. Jackson, Il Nuovo Cimento, Siere X, Vol 34, pag 1644-1666
  // 16 Dicembre 1964
  // equation 10. (refers to (4)).
  //
  double m1 = daughterPDGMass(0);
  double m2 = daughterPDGMass(1);

  double m1s = m1*m1;
  double m2s = m2*m2;

  double om = mumsMass();
  double om2 = om*om;
  double om4 = om2*om2;

  double G  = mumsWidth();

  double mpm = m1s + m2s;
  double mmm = m1s - m2s;
  double mmm2 = mmm*mmm;

  double num = om4 - mmm2;
  double den = om4 - 2*mpm*om2 + mmm2;

  double f1 = ((double) twoLPlusOne())/8.0;
  double f2 = G*G/om;
  double f3 = num/den;
  
  return f1*f2*f3;
}
double BW_BW::peakPosition() const{
  // from: J.D. Jackson, Il Nuovo Cimento, Siere X, Vol 34, pag 1644-1666
  // 16 Dicembre 1964
  // equation 10. (refers to (4)).
  //
  return mumsMass() + peakShift();
}

void BW_BW::makeGeneratingFunction() const{
    if(! _genFct){
        if(nonResonant()){
            counted_ptr<IGenFct> gptr(new FlatFct(getDalitzCoordinate()));
            _genFct = gptr;
        }
        else if (_theDecay.nDgtr()==2 && (_theDecay.getDgtrVal(0).isNonResonant() || _theDecay.getDgtrVal(1).isNonResonant()) ){
            counted_ptr<IGenFct> gptr(new BWFct(getDalitzCoordinate()
                                                , mumsMass()
                                                , mumsWidth() * 1.02 ));
            _genFct = gptr;
        }else{
            counted_ptr<IGenFct> gptr(new BWFct(getDalitzCoordinate()
                                                , peakPosition()
                                                , mumsWidth() * 1.02 ));
            _genFct = gptr;
        }
    }
}

counted_ptr<IGenFct> BW_BW::generatingFunction() const{
  if(! _genFct){
    makeGeneratingFunction();
  }
  return _genFct;
}

void BW_BW::print(std::ostream& out)const {
  out << name();
}
void BW_BW::print(IDalitzEvent& evt, std::ostream& out) {
  setEventPtr(evt);
  resetInternals();
  out << name()
      << "\n\t> co-ordinate: " << getDalitzCoordinate()
      << "\n\t> This is the decay I'm looking at:"
      << "\n" << _theDecay
      << "\n\t> These are a few values: "
      << " startOfDecayChain? " << startOfDecayChain()
      << ", BreitWigner = " 
      << BreitWigner()
      << ", Blatt-Weisskopf penetration factor: "
      << Fr()
      << ", total BW_BW: " 
      << getVal(evt);
}

std::complex<double> BW_BW::EvtGenValue(IDalitzEvent& evt){
  // For debugging only.  
  // This is the EvtGen version of the full 3-body amplitude
  // (cut'n'paste with minimal changes).  
  // BW_BW only returns only the Breit Wigner plus form factor for one
  // resonance. So don't compare with BW_BW, but with Amplitude.  This
  // function is part of BW_BW for convencience, BW_BW provides all
  // the "services" needed to compute this.

  setEventPtr(evt);
  double pi180inv = pi/108.0;//1.0/EvtConst::radToDegrees;

  complex<double> ampl(0,0);
  double magn =1;
  double theta = 0;
  
  int spin = (twoLPlusOne() -1)/2;

  // get parent
  if(0 == _theDecay.hasParent()) {
    return -9999;
  }  
  TLorentzVector p4_p = getEvent()->p(0)  * (1./GeV);
  TLorentzVector p4_d1 = getEvent()->p(_theDecay.getDgtrTreePtr(0)->getVal().asi()[0])   * (1./GeV);
  TLorentzVector p4_d2 = getEvent()->p(_theDecay.getDgtrTreePtr(1)->getVal().asi()[0])   * (1./GeV);
  TLorentzVector p4_d3 = p4_p-p4_d1-p4_d2;

  //get cos of the angle between the daughters from their 4-momenta
  //and the 4-momentum of the parent

  //in general, EvtDecayAngle(parent, part1+part2, part1) gives the angle
  //the missing particle (not listed in the arguments) makes
  //with part2 in the rest frame of both
  //listed particles (12)
 
  //angle 3 makes with 2 in rest frame of 12 (CS3)  
  //double cos_phi_0 = EvtDecayAngle(p4_p, p4_d1+p4_d2, p4_d1);
  //angle 3 makes with 1 in 12 is, of course, -cos_phi_0

  //first compute several quantities...follow CLEO preprint 00-23

  bool invmass_angdenom = false;
  double bwm = mumsMass()  / GeV;
  double gamma = mumsWidth()  / GeV;

  double mAB=(p4_d1+p4_d2).M();
  double mBC=(p4_d2+p4_d3).M();
  double mAC=(p4_d1+p4_d3).M();
  double mA=p4_d1.M(); 
  double mB=p4_d2.M(); 
  double mD=p4_p.M();
  double mC=p4_d3.M();
  
  double mR=bwm;
  double mdenom = invmass_angdenom ? mAB : mR;
  double gammaR = gamma;
  double pAB=sqrt( (((mAB*mAB-mA*mA-mB*mB)*(mAB*mAB-mA*mA-mB*mB)/4.0) -
		    mA*mA*mB*mB)/(mAB*mAB));
  double pR=sqrt( (((mR*mR-mA*mA-mB*mB)*(mR*mR-mA*mA-mB*mB)/4.0) -
		   mA*mA*mB*mB)/(mR*mR));

  double pD= (((mD*mD-mR*mR-mC*mC)*(mD*mD-mR*mR-mC*mC)/4.0) -
		   mR*mR*mC*mC)/(mD*mD);
  if ( pD>0 ) { pD=sqrt(pD); } else {pD=0;}
  double pDAB=sqrt( (((mD*mD-mAB*mAB-mC*mC)*(mD*mD-mAB*mAB-mC*mC)/4.0) -
		   mAB*mAB*mC*mC)/(mD*mD));


  //  report(INFO,"EvtGen")  << mAB<<" "<< mBC<<" "<< mAC<<" "<< mA<<" "<< mB<<" "<< mC<<" "
  //		<< mD<<" "<< mR<<" "<< gammaR<<" "<< pAB<<" "<< pR<<" "<< pD<<" "<<pDAB<<endl;

  double fR=1;
  double fD=1;
  int power=0;
  switch (spin) {
  case 0:
    fR=1.0;
    fD=1.0;
    power=1;
    //report(INFO,"EvtGen") << "fR="<<fR<<" fD="<<fD<<endl;
    break;
  case 1:
    fR=sqrt(1.0+1.5*1.5*pR*pR)/sqrt(1.0+1.5*1.5*pAB*pAB);
    fD=sqrt(1.0+5.0*5.0*pD*pD)/sqrt(1.0+5.0*5.0*pDAB*pDAB);
    //report(INFO,"EvtGen") << "fR="<<fR<<" fD="<<fD<<endl;
    power=3;
    break;
  case 2:
    fR = sqrt( (9+3*pow((1.5*pR),2)+pow((1.5*pR),4))/(9+3*pow((1.5*pAB),2)+pow((1.5*pAB),4)) );
    fD = sqrt( (9+3*pow((5.0*pD),2)+pow((5.0*pD),4))/(9+3*pow((5.0*pDAB),2)+pow((5.0*pDAB),4)) );
    power=5;
    //report(INFO,"EvtGen") << "fR="<<fR<<" fD="<<fD<<std::endl;
    break;

    

  default:
    ;//report(INFO,"EvtGen") << "Incorrect spin in EvtResonance22.cc\n";
  }

  cout << "EvtGen: fR = " << fR << ", fD = " << fD << endl;

  //  report(INFO,"EvtGen") << "fR = " << fR << " fD = " << fD << endl;
  
  double gammaAB= gammaR*pow(pAB/pR,power)*(mR/mAB)*fR*fR;
  cout << "EvtGen: gamma AB = " << gammaAB << endl;
  //report(INFO,"EvtGen") << gammaAB<<endl;
  double sf;
  switch (spin) {
  case 0:
    ampl=magn*complex<double>(cos(theta*pi180inv),sin(theta*pi180inv))*
          fR*fD/(mR*mR-mAB*mAB-complex<double>(0.0,mR*gammaAB));
    sf=1;
    break;
  case 1:
    ampl=magn*complex<double>(cos(theta*pi180inv),sin(theta*pi180inv))*
      (fR*fD*(mAC*mAC-mBC*mBC+((mD*mD-mC*mC)*(mB*mB-mA*mA)/(mdenom*mdenom)))/
       (mR*mR-mAB*mAB-complex<double>(0.0,mR*gammaAB)));
    
    sf = mAC*mAC-mBC*mBC+((mD*mD-mC*mC)*(mB*mB-mA*mA)/(mdenom*mdenom));
    cout << "EvtGen: sf spin 1 : " <<  sf << endl;
    break;
  case 2:
//     ampl=magn*complex<double>(cos(theta*pi180inv),sin(theta*pi180inv))*
//	fR*fD/(mR*mR-mAB*mAB-complex<double>(0.0,mR*gammaAB))*
//        (pow((mBC*mBC-mAC*mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(mAB*mAB)),2)-
//	(1.0/3.0)*(mAB*mAB-2*mD*mD-2*mC*mC+pow((mD*mD- mC*mC)/mAB, 2))*
//	(mAB*mAB-2*mA*mA-2*mB*mB+pow((mA*mA-mB*mB)/mAB,2)));
     ampl=magn*complex<double>(cos(theta*pi180inv),sin(theta*pi180inv))*
	fR*fD/(mR*mR-mAB*mAB-complex<double>(0.0,mR*gammaAB))*
        (pow((mBC*mBC-mAC*mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(mdenom*mdenom)),2)-
	(1.0/3.0)*(mAB*mAB-2*mD*mD-2*mC*mC+pow((mD*mD- mC*mC)/mdenom, 2))*
	(mAB*mAB-2*mA*mA-2*mB*mB+pow((mA*mA-mB*mB)/mdenom,2)));
     
     sf =  pow((mBC*mBC-mAC*mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(mdenom*mdenom)),2)-
       (1.0/3.0)*(mAB*mAB-2*mD*mD-2*mC*mC+pow((mD*mD- mC*mC)/mdenom, 2))*
       (mAB*mAB-2*mA*mA-2*mB*mB+pow((mA*mA-mB*mB)/mdenom,2));
     cout << "EvtGen: sf spin 2 " << sf << endl;
   break;

  default:
    ;//report(INFO,"EvtGen") << "Incorrect spin in EvtResonance22.cc\n";
  }

  //report(INFO,"EvtGen") <<"The amplitude is "<<ampl<<endl;
  return ampl;

}

std::ostream& operator<<(std::ostream& out, const BW_BW& amp){
  amp.print(out);
  return out;
}


//
