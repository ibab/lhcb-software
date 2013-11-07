// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzBWArea.h"

#include "Mint/TGenPhaseSpaceWithRnd.h"
#include "Mint/phaseSpaceIntegrals.h"

#include "Mint/FlatFct.h"
#include "Mint/IGenFct.h"

#include "Mint/DalitzEvent.h"
#include "Mint/Calculate4BodyProps.h"

#include "Mint/Utils.h"

#include "TRandom.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace MINT;

DalitzBWArea::DalitzBWArea()
  : _pat()
  , _rnd(gRandom)
  , _madeCMap(false)
  , _areaIntegral(-9999.0)
  , _unWeightPs(false)
  , _coords()
  , _mappedCoords()
{
  makeCoords();
}

DalitzBWArea::DalitzBWArea(const DalitzBWArea& other)
  : _pat(other._pat)
  , _rnd(other._rnd)
  , _madeCMap(false)
  , _areaIntegral(other._areaIntegral)
  , _unWeightPs(other._unWeightPs)
  , _coords(other._coords)
  , _mappedCoords(other._mappedCoords)
{
}

DalitzBWArea& DalitzBWArea::operator=(const DalitzBWArea& other){
  _pat = other._pat;
  _rnd = other._rnd;
  _madeCMap = false;
  _coords = other._coords;
  _mappedCoords = other._mappedCoords;
  _areaIntegral = other._areaIntegral;
  _unWeightPs = other._unWeightPs;
  return (*this);
}

DalitzBWArea::DalitzBWArea(const DalitzEventPattern& pat, TRandom* rnd)
  : _pat(pat)
  , _rnd(rnd)
  , _madeCMap(false)
  , _areaIntegral(-9999.0)
  , _unWeightPs(false)
  , _coords()
  , _mappedCoords()
{
  makeCoords();
  
  if(_pat.numDaughters() != 4
      && _pat.numDaughters() != 3
    ){
    cout << "DalitzBWArea::DalitzBWArea:"
	 << "\n  Sorry, can only deal with 3 and 4 body decays. "
	 << "\n  Please improve me so I can deal with decays like this: " 
	 << pat << endl;
    cout << "  I'll crash now." << endl;
    throw "DalitzBWArea: \"I'm no superman\"";
  }
  makeMiMa();
}

void DalitzBWArea::setPattern(const  DalitzEventPattern& pat){
  _pat = pat;
  makeCoords();
  if(_pat.numDaughters() != 4
     && _pat.numDaughters() != 3
     ){
    cout << "DalitzBWArea::DalitzBWArea:"
	 << "\n  Sorry, can only deal with 3 or 4 body decays. "
	 << "\n  Please improve me so I can deal with decays like this: " 
	 << pat << endl;
    cout << "  I'll crash now." << endl;
    throw "DalitzBWArea: \"I'm no superman\"";
  }
  makeMiMa();
}

bool DalitzBWArea::setRnd(TRandom* rnd){
  _rnd = rnd;
  return true;
}

void DalitzBWArea::makeCoords(){
  _coords.clear();
  if(_pat.numDaughters() >= 4)makeCoord(2,3,4);
  makeCoord(1,2);
  makeCoord(2,3);
  if(_pat.numDaughters() >= 4) makeCoord(3,4);
  if(_pat.numDaughters() >= 4) makeCoord(1,2,3);
}
void DalitzBWArea::makeCoord(int i, int j){
  DalitzCoordinate c(i, j);
  counted_ptr<IGenFct> fcn(new FlatFct(c));
  std::pair<DalitzCoordinate, counted_ptr<IGenFct> > p (c, fcn);
  _coords[c.myKey()] = p;
}
void DalitzBWArea::makeCoord(int i, int j, int k){
  DalitzCoordinate c(i, j, k);
  counted_ptr<IGenFct> fcn(new FlatFct(c));
  std::pair<DalitzCoordinate, counted_ptr<IGenFct> > p (c, fcn);
  _coords[c.myKey()] = p;
}

void DalitzBWArea::setFcn(const DalitzCoordinate& c, const counted_ptr<IGenFct>& fct){
  fct->setLimits(sf(c).first.min(), sf(c).first.max());
  sf(c).second = fct;
}

void DalitzBWArea::makeMiMa(){
  bool dbThis=false;
  double safetyFactor=1; // for debug only - otherwise 1
  //  double safetyFactor=1.00001;
  s12().setMinMax( _pat.sijMin(1,2)/safetyFactor
		  , _pat.sijMax(1,2)*safetyFactor);
  s23().setMinMax( _pat.sijMin(2,3)/safetyFactor
		  , _pat.sijMax(2,3)*safetyFactor);
  if(dbThis) cout << "_pat.numDaughters() " << _pat.numDaughters()  << endl;
  if(_pat.numDaughters() >= 4){
    t01().setMinMax( _pat.sijMin(2,3,4)/safetyFactor
		    , _pat.sijMax(2,3,4)*safetyFactor);
    s34().setMinMax( _pat.sijMin(3,4)/safetyFactor
		    , _pat.sijMax(3,4)*safetyFactor);
    t40().setMinMax( _pat.sijMin(1,2,3)/safetyFactor
		    , _pat.sijMax(1,2,3)*safetyFactor);
  }
  f_s12()->setLimits( _pat.sijMin(1,2)/safetyFactor
		      , _pat.sijMax(1,2)*safetyFactor);
  f_s23()->setLimits( _pat.sijMin(2,3)/safetyFactor
		      , _pat.sijMax(2,3)*safetyFactor);
  if(_pat.numDaughters() >= 4){
    f_t01()->setLimits( _pat.sijMin(2,3,4)/safetyFactor
			, _pat.sijMax(2,3,4)*safetyFactor);
    f_s34()->setLimits( _pat.sijMin(3,4)/safetyFactor
			, _pat.sijMax(3,4)*safetyFactor);
    f_t40()->setLimits( _pat.sijMin(1,2,3)/safetyFactor
			, _pat.sijMax(1,2,3)*safetyFactor);
  }

}

DalitzBWArea::~DalitzBWArea(){
}

std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& 
DalitzBWArea::sf(const DalitzCoordinate& c){
  map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::iterator it = _coords.find(c.myKey());
  if(it == _coords.end()){
    cout << "ERROR in DalitzBWArea::sf"
	 << " unknown coordinate: " << c
	 << endl;
    throw "CAN'T HANDLE THAT...";
  }
  return it->second;
}

const std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& 
DalitzBWArea::sf(const DalitzCoordinate& c) const{
  map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::iterator it = 
    _coords.find(c.myKey());
  if(it == _coords.end()){
    cout << "ERROR in DalitzBWArea::sf"
	 << " unknown coordinate: " << c
	 << endl;
    throw "CAN'T HANDLE THAT...";
  }
  return it->second;
}

std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& 
DalitzBWArea::sf(int i, int j, int k){
  DalitzCoordinate c(i,j,k);
  return sf(c);
}

const std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& 
DalitzBWArea::sf(int i, int j, int k) const{
  DalitzCoordinate c(i,j,k);
  return sf(c);
}

std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& DalitzBWArea::sf(int i, int j){
  DalitzCoordinate c(i,j);
  return sf(c);
}

const std::pair<DalitzCoordinate, counted_ptr<IGenFct> >& DalitzBWArea::sf(int i, int j) const{
  DalitzCoordinate c(i,j);
  return sf(c);
}

bool DalitzBWArea::isInside(const DalitzEvent& evt) const{
  Permutation mapping(evt.eventPattern().size());
  mapping.makeUnity();
  return isInside(evt, mapping);
}

bool DalitzBWArea::isInside(const DalitzEvent& evt
			    , const Permutation& mapping) const{
  bool dbThis=false;
  //cout << "Hello from  DalitzBWArea::isInside" << endl;
  if(mapping.mapOrder(_pat) != evt.eventPattern()){
    if(dbThis) cout << "patterns don't match in isInside" << endl;
    return false;
  }
  //cout << " Made Coordinate map " << endl;
  std::vector<int> mappedValues;
  for(map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    if(dbThis) {
      cout << "old: " << evt.sij(it->second.first)
	   << "  new " << evt.sij(mapping.mapValues(it->second.first))
	   << "  newer " << evt.sij(it->second.first.mapMe(mapping))
	   << endl;
   }
    // the following line is instead of the also possible, nicer looking, 
    // but malloc-intensive evt.sij(mapping.mapValues(it->second.first))
    mapping.mapValues(it->second.first, mappedValues);
    double val = evt.sij(mappedValues);
    if(val < it->second.first.min() || val >= it->second.first.max()) return false;
  }
  //cout << "returning true" << endl;
  return true;
}
bool DalitzBWArea::isInside(const DalitzCoordinate& dc) const{

  //cout << " Made Coordinate map " << endl;
  double val = dc.val();
  map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator it = _coords.find(dc);
  if(it == _coords.end()){
    cout << "ERROR in DalitzBWArea::isInside - unknown coordinate "
	 << dc << endl;
    return false;
  }
  if(it->second.first.min() <= val && it->second.first.max() > val){
    return true;
  }else{
    return false;
  }
}
bool DalitzBWArea::isInside(const std::vector<DalitzCoordinate>& dcList) const{
  for(unsigned int i=0; i < dcList.size(); i++){
    if(! isInside(dcList[i])){
      return false;
    }
  }
  return true;
}

double DalitzBWArea::size() const{
  if(_pat.size() < 3) return 0;

  double p=1;
  
  /*  cout << " DalitzBWArea::size(): returning..." << endl;
  cout << " e.g. _t01 " << _t01 << endl;
  cout << " e.g. _s12 " << _s12 << endl;
  cout << "_madeCMap = " << _madeCMap << endl;
  */
  for(map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator it = _coords.begin();
      it != _coords.end(); it++){

    p *= (it->second.first.max() -  it->second.first.min());
  }
  return fabs(p);
}

/*
double DalitzBWArea::genValue(const IDalitzEvent* evtPtr) const{
  if(0 == evtPtr) return 0;

  Permutation mapping(evtPtr->eventPattern().size());
  mapping.makeUnity();
  return genValue(evtPtr, mapping);
}
*/
double DalitzBWArea::genValue(const IDalitzEvent* evtPtr) const{
  //bool dbThis=false;
  if(0 == evtPtr) return 0;
  double p=1;

  for(map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    //DalitzCoordinate c =  it->second.first;
    counted_ptr<IGenFct> fct(it->second.second);

    p *= fct->generatingFctValue( evtPtr->sij(it->second.first) );
  }
  return fabs(p);
}
double DalitzBWArea::genValue(const IDalitzEvent* evtPtr
			      , const Permutation& mapping) const{
  bool dbThis=false;

  if(0 == evtPtr) return 0;

  /*
    we save significant time by not doing this check (below) - so let's risk it.
 
  if(mapping.mapOrder(_pat) != evtPtr->eventPattern()){
    cout << "ERROR in DalitzBWArea::genValue(const IDalitzEvent* evtPtr) const"
	 << " patterns don't match!"
	 << "    compare: " << _pat << "   and   " 
	 << evtPtr->eventPattern() << endl;
    return 0;
  }
  */
  

  double p=1;

  for(map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    DalitzCoordinate c =  it->second.first;
    counted_ptr<IGenFct> fct(it->second.second);


    if(dbThis){
      cout << "DalitzBWArea::genValue: factor for variable "
	   << c
	   << " s with mapping = " << evtPtr->sij(mapping.mapValues(c))
	   << " s with new mapping = " << evtPtr->sij(c.mapMe(mapping))
	   << ", and without= " << evtPtr->sij(c)
	   << " fct->generatingPDFValue with mapping "
	   << fct->generatingFctValue( evtPtr->sij(mapping.mapValues(c)) )
	   << " new mapping "
	   << fct->generatingFctValue( evtPtr->sij(c.mapMe(mapping)) )
	   << ", and without: "
	   << fct->generatingFctValue( evtPtr->sij(c) )
	   << endl;
    }

    p *= fct->generatingFctValue( evtPtr->sij(it->second.first.mapMe(mapping)));
    if(dbThis) cout << " p = " << p << endl;
  }
  if(dbThis) cout << " returning p " << p << endl;

  return fabs(p);
}

double DalitzBWArea::genValueRho(const IDalitzEvent* evtPtr) const{
  // for debugging only.
  bool dbThis=false;
  
  if(0 == evtPtr) return 0;
  if(_pat != evtPtr->eventPattern()){
    cout << "ERROR in DalitzBWArea::genValue(const IDalitzEvent* evtPtr) const"
	 << " patterns don't match!"
	 << "    compare: " << _pat << "   and   " 
	 << evtPtr->eventPattern() << endl;
    return 0;
  }
  double p=1;

  for(map<DalitzCoordKey, pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    DalitzCoordinate c =  it->second.first;
    counted_ptr<IGenFct> fct(it->second.second);
    double rho = fct->coordTransformFromS(evtPtr->sij(c));
    if(dbThis)cout << "DalitzBWArea::genValueRho: factor for variable "
		   << c
		   << " s = " << evtPtr->sij(c)
		   << " fct->generatingPDFValue "
		   << fct->transformedFctValue(rho )
		   << endl;
   p *= fct->transformedFctValue(rho );
    if(dbThis) cout << " p = " << p << endl;
  }
  if(dbThis) cout << " returning p " << p << endl;

  return fabs(p);
}

double DalitzBWArea::integral() const{
  //bool dbThis=false;
  if(_areaIntegral < 0){
    
    if(_pat.numDaughters() == 3){
      _areaIntegral =  integral3();
    }else if(_pat.numDaughters() == 4){
      _areaIntegral =  integral4();
    }else{
      cout << "ERROR in DalitzBWArea::integral() can only make events"
	   << " with 3 or 4 daughters. You want : " << _pat
	   << endl;
      _areaIntegral = 0;
    }
  }
  return _areaIntegral;
}
  
counted_ptr<DalitzEvent> DalitzBWArea::tryEventForOwner(const Permutation& mapping) const{
  bool dbThis = false;
  if(_pat.numDaughters() == 3) return try3Event(mapping);
  if(_pat.numDaughters() == 4){
    counted_ptr<DalitzEvent> evtPtr(try4Event(mapping));
    if(dbThis && 0 != evtPtr) cout << " DalitzBWArea::makeEventForOwner() "
				   << " returning event with weight " 
				   << evtPtr->getWeight()
				   << endl;
    if(0 != evtPtr && _pat[0] < 0) evtPtr->P_conjugateYourself();
    // the above ensures that, for the same random seed,
    // identical but CP conjugate events are generated
    // for D->f and Dbar->fbar.
    // Note that this step of the event generation is
    // completely P-even, and the event generation would
    // still be correct without this P-conjugation. The 
    // crucial P-senstive step is the reweighting applied 
    // later, which will then take into account the full
    // amplitude model. The P-conjugation here is just to keep the
    // random numbers in sync between CP conjugate event generations.
    return evtPtr;
  }
  cout << "ERROR in DalitzBWArea::tryEventForOwner() can only make events"
       << " with 3 or 4 daughters. You want : " << _pat
       << endl;
  return counted_ptr<DalitzEvent>(0);
}

counted_ptr<DalitzEvent> DalitzBWArea::try3Event(const Permutation& mapping) const{
  bool dbThis=false;

  counted_ptr<DalitzEvent> evtPtr(try3EventWithPhaseSpace(mapping));
  if(dbThis && 0 != evtPtr){
    cout << "weight in DalitzBWArea::make3Event() " 
	 << evtPtr->getWeight() << endl;
  }
  return evtPtr;
}

counted_ptr<DalitzEvent> DalitzBWArea::try4Event(const Permutation& mapping) const{
  bool dbThis=false;

  counted_ptr<DalitzEvent> evtPtr(0);
  if(unWeightPs()){
    evtPtr = make4EventWithPhaseSpace(mapping);
  }else{
    evtPtr = try4EventWithPhaseSpace(mapping);
  }
  if(dbThis && 0 != evtPtr){
    cout << "weight in DalitzBWArea::make4Event() " 
	 << evtPtr->getWeight() << endl;
  }
  return evtPtr;
  //  return make4EventWithPhaseSpace();

}

counted_ptr<DalitzEvent> DalitzBWArea::make4EventWithPhaseSpace(const Permutation& mapping) const{
  double nt;
  return make4EventWithPhaseSpace(nt, mapping);
}
counted_ptr<DalitzEvent> DalitzBWArea::make4EventWithPhaseSpace(double& nTries
								, const Permutation& mapping) const{
  int nearInfinity = 100000000;
  counted_ptr<DalitzEvent> evtPtr(0);
  nTries=0;
  double maxW=-9999;
  do{
    evtPtr = try4EventWithPhaseSpace(maxW, mapping);
    nTries++;
    if(nTries > nearInfinity){
      cout << "DalitzBWArea::makeEventWithPhaseSpace() "
	   << " giving up after " << nTries
	   << " unsuccessful tries." << endl;
      return evtPtr;
    }
  }while(0 == evtPtr || _rnd->Rndm()*maxW > evtPtr->getWeight());
  evtPtr->setWeight(1);

  return evtPtr;
}

double DalitzBWArea::MC4Integral(double& precision)const{
  // for cross checks only
  int N=1000000;
  double sum=0;
  double sumsq=0;
  for(int i=0; i < N; i++){
    counted_ptr<DalitzEvent> evtPtr(try4EventWithPhaseSpace());
    if(0 == evtPtr) continue;
    double val = evtPtr->getWeight();// * genValueRho(&(*evtPtr));
    sum += val;
    sumsq += val*val;
  }
  double mean = sum/((double)N);
  double meansq = sumsq/((double)N);
  double variance = (meansq - mean*mean)/((double)N);
  double sigma = sqrt(variance);
  precision = sigma/mean;
  
  return mean * integral();

  // ============== not used ==================
  double xmi=0, xma=0, ymi=0, yma=0;
  if(ResonanceConfigurationNumber() == 0){
    xmi = sf(1,2).second->getRhoMi();
    xma = sf(1,2).second->getRhoMa();
    ymi = sf(3,4).second->getRhoMi();
    yma = sf(3,4).second->getRhoMa();
  }else{
    xmi =  sf(1,2,3).second->getRhoMi();
    xma =  sf(1,2,3).second->getRhoMa();
    ymi =  sf(1,2).second->getRhoMi();
    yma =  sf(1,2).second->getRhoMa();
  }

  double intArea = (xma - xmi)*(yma - ymi);
  if(intArea < 0){
    cout << "DalitzBWArea::MC4Integral() negative area? "
	 << "\n   x: ( " << xma << " - " << xmi << ") = "
	 << xma - xmi
	 << "\n   y: ( " << yma << " - " << ymi << ") = "
	 << yma - ymi
	 << endl;
  }

  cout << "DalitzBWArea::MC4Integral: returning "
       << mean << " * " << intArea << endl;
  return mean * intArea;
  
}

double DalitzBWArea::MC4IntegralNoTransform(double& prec)const{
  // for cross checks only.
  int N=1000000;
  double sum=0;
  double sumsq=0;
  for(int i=0; i < N; i++){
    counted_ptr<DalitzEvent> evtPtr(tryFlat4EventWithPhaseSpace());
    if(0 == evtPtr) continue;
    double val = evtPtr->getWeight() * genValue(&(*evtPtr));
    sum += val;
    sumsq += val*val;
  }
  double mean = sum/((double)N);
  double meansq = sumsq/((double)N);
  double variance = (meansq - mean*mean)/((double)N);
  prec = sqrt(variance)/mean;

  double xmi=0, xma=0, ymi=0, yma=0;
  if(ResonanceConfigurationNumber() == 0){
    xmi = sf(1,2).second->getSMi();
    xma = sf(1,2).second->getSMa();
    if(_pat.numDaughters()>=4){
      ymi = sf(3,4).second->getSMi();
      yma = sf(3,4).second->getSMa();
    }
  }else{
    xmi =  sf(1,2,3).second->getSMi();
    xma =  sf(1,2,3).second->getSMa();
    if(_pat.numDaughters()>=4){
      ymi =  sf(1,2).second->getSMi();
      yma =  sf(1,2).second->getSMa();
    }
  }

  double intArea = 0;
  
  if(_pat.numDaughters()>=4){
    intArea = (xma - xmi)*(yma - ymi);
  }else{
    intArea = (xma - xmi);
  }
  if(intArea < 0){
    cout << "DalitzBWArea::MC4IntegralNoTransform() negative area? "
	 << "\n   x: ( " << xma << " - " << xmi << ") = "
	 << xma - xmi
	 << "\n   y: ( " << yma << " - " << ymi << ") = "
	 << yma - ymi
	 << endl;
  }

  cout << "DalitzBWArea::MC4IntegralNoTransform: returning "
       << mean << " * " << intArea << endl;
  return mean * intArea;
  
}

bool DalitzBWArea::checkIntegration()const{
  
  cout << " DalitzBWArea::checkIntegration() Checking integration."
       << "\n    ResonanceConfigurationNumber " 
       << ResonanceConfigurationNumber()
       << "\n    Analytical: ";
  if(_pat.numDaughters() != 4){
    cout << "DalitzBWArea::checkIntegration() only works for 4-body decays"
	 << " returning 0." << endl;
    return 0;
  }

  double analytical=0;
  if(ResonanceConfigurationNumber() == 0){
    PhaseSpaceIntegral4bodyWith_s12s34 psi(&(*sf(1,2).second)
					   , &(*sf(3,4).second), _pat);
    
    analytical =  psi.getVal();
  }else{
    PhaseSpaceIntegral4bodyWith_s123s12 psi(&(*sf(1,2,3).second)
					    , &(*sf(1,2).second), _pat);
    analytical =  psi.getVal();
  }
  double prec_1=-9999, prec_2=-9999;

  double numerical_1 = MC4Integral(prec_1);
  double numerical_2 = MC4IntegralNoTransform(prec_2);

  cout << "DalitzBWArea::checkIntegration(): Result:"
       << "\n\t      root's integral " << analytical
       << "\n\t        MC with trafo " << numerical_1
       << " +/- " << prec_1 * numerical_1
       << "\n\t        MC w/o trafo  " << numerical_2
       << " +/- " << prec_2 * numerical_2
       << endl;
  return true;
    
}

double DalitzBWArea::integral4() const{
  bool dbThis=false;

  if(_pat.numDaughters() != 4){
    cout << "DalitzBWArea::integral4() called, although this pattern: "
	 << _pat
	 << " is a " << _pat.numDaughters() << " body decay."
	 << " I'll crash now." << endl;
    throw "wrong integral4";
  }
  if(dbThis){
    if(sf(1,2).second->flat() && sf(3,4).second->flat()){
      cout << "check this integral: " << endl;
      PhaseSpaceIntegral4bodyWith_s12s34 psiA(&(*sf(1,2).second)
					      , &(*sf(3,4).second), _pat);
      double vA = psiA.getVal();
      cout << " 12, 34: " << vA << endl;
      PhaseSpaceIntegral4bodyWith_s123s12 psiB(&(*sf(1,2,3).second)
					       , &(*sf(1,2).second), _pat);
      double vB = psiB.getVal();
      cout << " 123, 12: " << vB << endl;
      PhaseSpaceIntegral4body psiC;
      double vC = psiC.getVal(_pat);
      cout << " tested " << vC << endl;
      cout << " check integrals: "
	   << vA << " , " << vB << " , " << vC << endl;
    }
  }

  // below:
  // unWeightPs() == true: We produce events of weight 1 according to
  // PDF =  BW * phase-space-density
  // this means, to get the relative normalisations of the different
  // Breit Wigners right (i.e. the probability that this particular 
  // instance of DalitzBWArea is called to produce an event), we
  // need to calculate the integral WITH  phase space, i.e. 
  // integral BreitWigner * phase-space-density dt01 ds12
  // or 
  // integral BreitWigner * phase-space-density ds12 ds23
  // depending on the resonance.
  //
  // unWeightPs() == false: we produce events of weight phase-space-density according to
  // PDF = BW
  // this means, to get the relative normalisations of the different
  // Breit Wigners right (i.e. the probability that this particular 
  // instance of DalitzBWArea is called to produce an event), we
  // need to calculate simply the integral WITHOUT  phase space, i.e. 
  // integral BreitWigner * ds123 ds12
  // or 
  // integral BreitWigner * ds12 ds34
  // which is much easier.
  
  if(ResonanceConfigurationNumber() == 0){
    if(unWeightPs()){
      PhaseSpaceIntegral4bodyWith_s12s34 psi(&(*sf(1,2).second)
					     , &(*sf(3,4).second), _pat);
      return psi.getVal();
    }else{
      return sf(1,2).second->integral() *  sf(3,4).second->integral();
    }
  }else{
    if(unWeightPs()){
      PhaseSpaceIntegral4bodyWith_s123s12 psi(&(*sf(1,2,3).second)
					      , &(*sf(1,2).second), _pat);
      return psi.getVal();
    }else{
      return  sf(1,2,3).second->integral() *  sf(1,2).second->integral();
    }
  }
}

double DalitzBWArea::integral3()const{
  if(unWeightPs()){
    // See integral4() for details on the meaning of unWeightPs(). 
    // Here the integration is only over one parameter, s12, otherwise 
    // it's the same idea.
    cout << "Don't know how to calculate integral3WithPhaseSpace() "
	 << "with option \"unWeightPs\" - please implement this."
	 << "\n I will crash now."
	 << endl;
    throw "no unweighted ps for 3-body, yet";
  }else{
    return sf(1,2).second->integral();
  }
}

int DalitzBWArea::ResonanceConfigurationNumber() const{
  if(_pat.numDaughters() <= 3) return 0; // only one configurtion: D->X(d1, d2), d3
  if(_pat.numDaughters() >  4) return 0; // no known configuration

  // so it's 4 daughters, m1, m2, m3, m4.
  // 0: treat as D->X Y, with X->m1, m2; Y->m3,m4
  // 1: treat as D->X m4, with X->Y m3, with Y->m1, m2

  if((! sf(1,2).second->flat()) 
     && (! sf(3,4).second->flat())){
    return 0; // D-> 2 resonances
  }else if((! sf(1,2).second->flat()) 
	   && sf(3,4).second->flat() 
	   && sf(1,2,3).second->flat()){
    return 0; // D-> 1 resonance + 2 bodies
  }else if( sf(1,2).second->flat()  
	    && sf(3,4).second->flat() 
	    && sf(1,2,3).second->flat()){
    return 0; // D-> 4 body (non-resonant)
  }else{
    return 1; // D-> resonance + 1 body; resonance->another resonance + 1 body
  }
}

counted_ptr<DalitzEvent> DalitzBWArea::try4EventWithPhaseSpace(const Permutation& mapping) const{
  double maxWeight;
  return try4EventWithPhaseSpace(maxWeight, mapping);
}

counted_ptr<DalitzEvent> DalitzBWArea::tryFlat4EventWithPhaseSpace(const Permutation& mapping) const{
  double maxWeight;
  return tryFlat4EventWithPhaseSpace(maxWeight, mapping);
}


counted_ptr<DalitzEvent> 
DalitzBWArea::try4EventWithPhaseSpace(double& maxWeight
				      , const Permutation& mapping) const{
  //  return counted_ptr<DalitzEvent>(0);
  //}

  bool dbThis=false;

  if(dbThis){
    cout << " trying to make event with phase space for "
	 << *this
	 << endl;
  }
  // hoping to save some allocation time
  // through use of static:
  static TGenPhaseSpaceWithRnd gps(_rnd);
  static TLorentzVector mumsP4;
  mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());

  static vector<TLorentzVector> p4_final(5);
  static vector<TLorentzVector> p4_finalMapped(5);
  static DalitzEventPattern patMapped;

  static TLorentzVector p4_inter[4];

  counted_ptr<DalitzEvent> nullEvtPtr(0);
  counted_ptr<DalitzEvent> returnEvent(0);

  // all set up such that we know that only
  // s123, s12 can be filled, or s12, s34
  // (or single limits: s123 or s12)

  maxWeight = 1.0;


  if(ResonanceConfigurationNumber() == 0){
    // like D->K* rho, K*->Kpi, rho->pipi
    if(dbThis) cout << " making s12, s34 configuration " << endl;

    double rho12     = sf(1,2).second->generateRho(_rnd);
    double s12       = sf(1,2).second->coordTransformToS(rho12);

    if(s12 < 0) return nullEvtPtr;
    double m12 = sqrt(s12);
    if(m12 < _pat[1].mass() + _pat[2].mass()) return nullEvtPtr;

    double rho34     = sf(3,4).second->generateRho(_rnd);
    double s34       = sf(3,4).second->coordTransformToS(rho34);

    if(s34 < 0) return nullEvtPtr;
    double m34 = sqrt(s34);
    if(m34 < _pat[3].mass() + _pat[4].mass()) return nullEvtPtr;
    if(m12 + m34 > _pat[0].mass())return nullEvtPtr;
    
    double s12Min  = sf(1,2).second->getCoordinate().min();
    double s12Max  = sf(1,2).second->getCoordinate().max();
    double s34Min  = sf(3,4).second->getCoordinate().min();
    double s34Max  = sf(3,4).second->getCoordinate().max();

    
    double fct_12 = 1;//sf(1,2).second->transformedFctValue(rho12);
    double fct_34 = 1;//sf(3,4).second->transformedFctValue(rho34);
    
    double fct_12Max = 1;//sf(1,2).second->transformedFctMax();
    double fct_34Max = 1;//sf(3,4).second->transformedFctMax();

    maxWeight *= fct_12Max;
    maxWeight *= fct_34Max;

    Double_t dgt[2];
    
    // D->s12, s34
    
    dgt[0] = m12;
    dgt[1] = m34;
    mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());
    p4_final[0] = mumsP4;
    
    if(! gps.SetDecay(mumsP4, 2, dgt)) return nullEvtPtr;
    gps.Generate();
    /* Note:
       usually, gps.Generate() produces weighted events, and
       to get un-weighted ones (i.e.events with weight 1) one
       needs to do:
       while(rnd->Rndm() > gps.Generate());
       However, 2-body decays all have weight 1.

    */
    p4_inter[0] = *(gps.GetDecay(0));
    p4_inter[1] = *(gps.GetDecay(1));
    
    double ps1 = phaseSpaceIntegral2body(_pat[0].mass()
					, m12
					, m34);
    maxWeight *= phaseSpaceIntegral2body(_pat[0].mass()
					 ,sqrt(s12Min)
					 ,sqrt(s34Min));
    if(ps1 <=0 ) return nullEvtPtr;
    
    // s12 -> m1, m2
    dgt[0] = _pat[1].mass();
    dgt[1] = _pat[2].mass();
    if(! gps.SetDecay(p4_inter[0], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[1] = *(gps.GetDecay(0));
    p4_final[2] = *(gps.GetDecay(1));
    
    double ps2 = phaseSpaceIntegral2body(m12
					 , _pat[1].mass()
					 , _pat[2].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s12Max)
					 , _pat[1].mass()
					 , _pat[2].mass());
    
    if(ps2 <=0)return nullEvtPtr;
    
    // s34 -> m3, m4
    dgt[0] = _pat[3].mass();
    dgt[1] = _pat[4].mass();
    if(! gps.SetDecay(p4_inter[1], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[3] = *(gps.GetDecay(0));
    p4_final[4] = *(gps.GetDecay(1));
    
    double ps3 = phaseSpaceIntegral2body(m34
					 , _pat[3].mass()
					 , _pat[4].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s34Max)
					 , _pat[3].mass()
					 , _pat[4].mass());
    
    if(ps3 <= 0) return nullEvtPtr;
    
    double ps = ps1 * ps2 * ps3;
    
    double w = fct_12 * fct_34 * ps;

    mapP4(p4_final, mapping, p4_finalMapped);
    mapping.mapOrder(_pat, patMapped);
    counted_ptr<DalitzEvent> thisEvent(new DalitzEvent(patMapped, p4_finalMapped));
    if(thisEvent->makeCalculate4BodyProps().phaseSpaceFactor() <= 0.0){
      Calculate4BodyProps c4bp = thisEvent->makeCalculate4BodyProps();
      cout << "WARNING in DalitzBWArea::try4EventWithPhaseSpace (0):"
	   << "\n made 'good' event with bad phase space "
	   << c4bp.showPhaseSpaceFactorCalculation()
	   << "\n"  << *thisEvent
	   << "masses:\n";
      for(int i=0; i <= 4; i++) cout << thisEvent->p(i).M() << ", ";
      cout << "\n should be; \n";
      for(int i=0; i <= 4; i++) cout << _pat[i].mass() << ", ";
      cout <<"\n momentum sum ";
      TLorentzVector psum(.0,.0,.0,.0); 
      for(int i=1; i <=4; i++) psum += thisEvent->p(i);
      cout << psum;
      cout << "\n ps1 " << ps1 << ", ps2 " << ps2 << ", ps3 " << ps3
	   << "; ps " << ps;
      cout << endl;
      return nullEvtPtr;
    }
    thisEvent->setWeight(w);
    returnEvent = thisEvent;
  }else{
    if(dbThis) cout << " making s123, s12 configuration " << endl;
    // like D->K1 pi, K1->K* pi, K*->Kpi

    double rho123    = sf(1,2,3).second->generateRho(_rnd);
    double s123      = sf(1,2,3).second->coordTransformToS(rho123);
    if(s123 < 0) return nullEvtPtr;
    double m123 = sqrt(s123);
    if(m123 + _pat[4].mass() >  _pat[0].mass()) return nullEvtPtr;
    if(m123 <  _pat[1].mass() + _pat[2].mass() + _pat[3].mass()){
      return nullEvtPtr;
    }

    double rho12     = sf(1,2).second->generateRho(_rnd);
    double s12       = sf(1,2).second->coordTransformToS(rho12);
    if(s12 < 0) return nullEvtPtr;
    double m12 = sqrt(s12);
    if(m12 + _pat[3].mass() > m123)return nullEvtPtr;
    if(m12 + _pat[3].mass() + _pat[4].mass() > _pat[0].mass() ){
      return nullEvtPtr;
    }
    if(m12 < _pat[1].mass() + _pat[2].mass()) return nullEvtPtr;
    
    double s123Min = sf(1,2,3).second->getCoordinate().min();
    double s123Max = sf(1,2,3).second->getCoordinate().max();
    double s12Min  = sf(1,2).second->getCoordinate().min();
    double s12Max  = sf(1,2).second->getCoordinate().max();
    
    Double_t dgt[2];
    
    
    double fct_123 = 1;//sf(1,2,3).second->transformedFctValue(rho123);
    double fct_12  = 1;//sf(1,2).second->transformedFctValue(rho12);
    double fct_123Max = 1;//sf(1,2,3).second->transformedFctMax();
    double fct_12Max = 1;//sf(1,2).second->transformedFctMax();

    maxWeight *= fct_123Max;
    maxWeight *= fct_12Max;

    // D->s123, m4
    
    dgt[0] = m123;
    dgt[1] = _pat[4].mass();
    mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());
    p4_final[0] = mumsP4;
    
    if(! gps.SetDecay(mumsP4, 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_inter[0] = *(gps.GetDecay(0));
    p4_final[4] = *(gps.GetDecay(1));
    
    double ps1 = phaseSpaceIntegral2body(_pat[0].mass()
					, m123
					, _pat[4].mass());
    maxWeight *= phaseSpaceIntegral2body(_pat[0].mass()
					 ,sqrt(s123Min)
					 , _pat[4].mass());
    
    if(ps1 <=0) return nullEvtPtr;
    // s123 -> s12, m3
    dgt[0] = m12;
    dgt[1] = _pat[3].mass();
    if(! gps.SetDecay(p4_inter[0], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_inter[1] = *(gps.GetDecay(0));
    p4_final[3] = *(gps.GetDecay(1));
    
    double ps2 = phaseSpaceIntegral2body(m123
				  , m12
				  , _pat[3].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s123Max)
				     ,sqrt(s12Min)
				     , _pat[3].mass());
    if(ps2 <=0) return nullEvtPtr;
    
    // s12 -> m1, m2
    dgt[0] = _pat[1].mass();
    dgt[1] = _pat[2].mass();
    if(! gps.SetDecay(p4_inter[1], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[1] = *(gps.GetDecay(0));
    p4_final[2] = *(gps.GetDecay(1));
    
    double ps3 = phaseSpaceIntegral2body(m12
					 , _pat[1].mass()
					 , _pat[2].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s12Max)
					 , _pat[1].mass()
					 , _pat[2].mass());
    
    if(ps3 <=0) return nullEvtPtr;
    
    double ps = ps1 * ps2 * ps3;

    double w = fct_123 * fct_12 * ps;

    mapP4(p4_final, mapping, p4_finalMapped);
    mapping.mapOrder(_pat, patMapped);
    counted_ptr<DalitzEvent> thisEvent(new DalitzEvent(patMapped
						       , p4_finalMapped));
    if(thisEvent->makeCalculate4BodyProps().phaseSpaceFactor() <= 0.0){
      cout << "WARNING in DalitzBWArea::try4EventWithPhaseSpace (2): "
	   << " made 'good' event with bad phase space"
	   << " for sqrt(s123) " << sqrt(s123) << " = " << m123
	   << " , sqrt(s12) " << sqrt(s12) << " = " << m12
	   << " , sqrt(s12) + m3 " << sqrt(s12) + _pat[3].mass()
	   << " , sqrt(s123) + m4 " << sqrt(s123) + _pat[4].mass()
	   << " and m(D) " << _pat[0].mass()
	   << endl;
      cout << " compare " << s123 << ", " << thisEvent->t(4,0)
	   << " compare " << s12  << ", " << thisEvent->s(1,2)
	   << endl;
      cout << "compare mapping: " << endl;
      for(unsigned int i=0; i < p4_final.size() && i < p4_finalMapped.size(); i++){
	cout << "premapping " << p4_final[i] << "  post: " << p4_finalMapped[i] << endl;
      }
      return nullEvtPtr;
    }
    thisEvent->setWeight(w);
    returnEvent = thisEvent;
  }
  if(dbThis)cout << " got new event with weight " 
		 << returnEvent->getWeight() << endl;
  return returnEvent;
}

counted_ptr<DalitzEvent> DalitzBWArea::tryFlat4EventWithPhaseSpace(double& maxWeight, const Permutation& mapping) const{
  //  return counted_ptr<DalitzEvent>(0);
  //}

  bool dbThis=false;

  if(dbThis){
    cout << " trying to make event with phase space for "
	 << *this
	 << endl;
  }  
  TGenPhaseSpaceWithRnd gps(_rnd);

  TLorentzVector mumsP4;
  mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());

  static vector<TLorentzVector> p4_final(5), p4_finalMapped(5);
  static TLorentzVector p4_inter[4];
  static DalitzEventPattern patMapped;

  counted_ptr<DalitzEvent> nullEvtPtr(0);
  counted_ptr<DalitzEvent> returnEvent(0);

  // all set up such that we know that only
  // s123, s12 can be filled, or s12, s34
  // (or single limits: s123 or s12)

  maxWeight = 1.0;


  if(ResonanceConfigurationNumber() == 0){
    if(dbThis) cout << " making s12, s34 configuration " << endl;

    double s12Min  = sf(1,2).second->getCoordinate().min();
    double s12Max  = sf(1,2).second->getCoordinate().max();
    double s34Min  = sf(3,4).second->getCoordinate().min();
    double s34Max  = sf(3,4).second->getCoordinate().max();

    double s12 = s12Min + _rnd->Rndm()*(s12Max - s12Min);

    if(s12 < 0) return nullEvtPtr;
    if(sqrt(s12) < _pat[1].mass() + _pat[2].mass()) return nullEvtPtr;

    double s34       = s34Min + _rnd->Rndm()*(s34Max - s34Min);

    if(s34 < 0) return nullEvtPtr;
    if(sqrt(s34) < _pat[3].mass() + _pat[4].mass()) return nullEvtPtr;
    if(sqrt(s12) + sqrt(s34) > _pat[0].mass())return nullEvtPtr;
    
    double fct_12 = 1;//sf(1,2).second->transformedFctValue(rho12);
    double fct_34 = 1;//sf(3,4).second->transformedFctValue(rho34);
    
    double fct_12Max = 1;//sf(1,2).second->transformedFctMax();
    double fct_34Max = 1;//sf(3,4).second->transformedFctMax();

    maxWeight *= fct_12Max;
    maxWeight *= fct_34Max;

    Double_t dgt[2];
    
    // D->s12, s34
    
    dgt[0] = sqrt(s12);
    dgt[1] = sqrt(s34);
    mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());
    p4_final[0] = mumsP4;
    
    if(! gps.SetDecay(mumsP4, 2, dgt)) return nullEvtPtr;
    gps.Generate();
    /* Note:
       usually, gps.Generate() produces weighted events, and
       to get un-weighted ones (i.e.events with weight 1) one
       needs to do:
       while(rnd->Rndm() > gps.Generate());
       However, 2-body decays all have weight 1.

    */
    p4_inter[0] = *(gps.GetDecay(0));
    p4_inter[1] = *(gps.GetDecay(1));
    
    double ps1 = phaseSpaceIntegral2body(_pat[0].mass()
					, sqrt(s12)
					, sqrt(s34));
    maxWeight *= phaseSpaceIntegral2body(_pat[0].mass()
					 ,sqrt(s12Min)
					 ,sqrt(s34Min));
    if(ps1 <=0 ) return nullEvtPtr;
    
    // s12 -> m1, m2
    dgt[0] = _pat[1].mass();
    dgt[1] = _pat[2].mass();
    if(! gps.SetDecay(p4_inter[0], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[1] = *(gps.GetDecay(0));
    p4_final[2] = *(gps.GetDecay(1));
    
    double ps2 = phaseSpaceIntegral2body(sqrt(s12)
					 , _pat[1].mass()
					 , _pat[2].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s12Max)
					 , _pat[1].mass()
					 , _pat[2].mass());
    
    if(ps2 <=0)return nullEvtPtr;
    
    // s34 -> m3, m4
    dgt[0] = _pat[3].mass();
    dgt[1] = _pat[4].mass();
    if(! gps.SetDecay(p4_inter[1], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[3] = *(gps.GetDecay(0));
    p4_final[4] = *(gps.GetDecay(1));
    
    double ps3 = phaseSpaceIntegral2body(sqrt(s34)
					 , _pat[3].mass()
					 , _pat[4].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s34Max)
					 , _pat[3].mass()
					 , _pat[4].mass());
    
    if(ps3 <= 0) return nullEvtPtr;
    
    double ps = ps1 * ps2 * ps3;
    
    double w = fct_12 * fct_34 * ps;

    mapP4(p4_final, mapping, p4_finalMapped);
    mapping.mapOrder(_pat, patMapped);
    counted_ptr<DalitzEvent> thisEvent(new DalitzEvent(patMapped, p4_finalMapped));
    if(thisEvent->makeCalculate4BodyProps().phaseSpaceFactor() <= 0.0){
      cout << "WARNING in tryFlat4EventWithPhaseSpace: "
	   << " made 'good' event with bad phase space"
	   << endl;
      return nullEvtPtr;
    }
    thisEvent->setWeight(w);
    returnEvent = thisEvent;
  }else{
    if(dbThis) cout << " making s123, s12 configuration " << endl;

    double s123Min = sf(1,2,3).second->getCoordinate().min();
    double s123Max = sf(1,2,3).second->getCoordinate().max();
    double s12Min  = sf(1,2).second->getCoordinate().min();
    double s12Max  = sf(1,2).second->getCoordinate().max();

    double s123      = s123Min + _rnd->Rndm()*(s123Max - s123Min);

    if(s123 < 0) return nullEvtPtr;
    if(sqrt(s123) + _pat[4].mass() >  _pat[0].mass()) return nullEvtPtr;
    if(sqrt(s123) < 
       _pat[1].mass() + _pat[2].mass() + _pat[3].mass()) return nullEvtPtr;
    

    double s12       = s12Min + _rnd->Rndm()*(s12Max - s12Min);

    if(s12 < 0) return nullEvtPtr;
    if(sqrt(s12) + _pat[3].mass() > sqrt(s123))return nullEvtPtr;
    if(sqrt(s12) + _pat[3].mass() + _pat[4].mass() > _pat[0].mass() )return nullEvtPtr;
    if(sqrt(s12) < _pat[1].mass() + _pat[2].mass()) return nullEvtPtr;
    
    
    Double_t dgt[2];
    
    
    double fct_123 = 1;//sf(1,2,3).second->transformedFctValue(rho123);
    double fct_12  = 1;//sf(1,2).second->transformedFctValue(rho12);
    double fct_123Max = 1;//sf(1,2,3).second->transformedFctMax();
    double fct_12Max = 1;//sf(1,2).second->transformedFctMax();

    maxWeight *= fct_123Max;
    maxWeight *= fct_12Max;

    // D->s123, m4
    
    dgt[0] = sqrt(s123);
    dgt[1] = _pat[4].mass();
    mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());
    p4_final[0] = mumsP4;
    
    if(! gps.SetDecay(mumsP4, 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_inter[0] = *(gps.GetDecay(0));
    p4_final[4] = *(gps.GetDecay(1));
    
    double ps1 = phaseSpaceIntegral2body(_pat[0].mass()
					, sqrt(s123)
					, _pat[4].mass());
    maxWeight *= phaseSpaceIntegral2body(_pat[0].mass()
					 ,sqrt(s123Min)
					 , _pat[4].mass());
    
    if(ps1 <=0) return nullEvtPtr;
    // s123 -> s12, m3
    dgt[0] = sqrt(s12);
    dgt[1] = _pat[3].mass();
    if(! gps.SetDecay(p4_inter[0], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_inter[1] = *(gps.GetDecay(0));
    p4_final[3] = *(gps.GetDecay(1));
    
    double ps2 = phaseSpaceIntegral2body(sqrt(s123)
				  , sqrt(s12)
				  , _pat[3].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s123Max)
				     ,sqrt(s12Min)
				     , _pat[3].mass());
    if(ps2 <=0) return nullEvtPtr;
    
    // s12 -> m1, m2
    dgt[0] = _pat[1].mass();
    dgt[1] = _pat[2].mass();
    if(! gps.SetDecay(p4_inter[1], 2, dgt)) return nullEvtPtr;
    gps.Generate();
    p4_final[1] = *(gps.GetDecay(0));
    p4_final[2] = *(gps.GetDecay(1));
    
    double ps3 = phaseSpaceIntegral2body(sqrt(s12)
					 , _pat[1].mass()
					 , _pat[2].mass());
    maxWeight *= phaseSpaceIntegral2body(sqrt(s12Max)
					 , _pat[1].mass()
					 , _pat[2].mass());
    
    if(ps3 <=0) return nullEvtPtr;
    
    double ps = ps1 * ps2 * ps3;

    double w = fct_123 * fct_12 * ps;

    mapP4(p4_final, mapping, p4_finalMapped);
    mapping.mapOrder(_pat, patMapped);

    counted_ptr<DalitzEvent> thisEvent(new DalitzEvent(patMapped, p4_finalMapped));
    if(thisEvent->makeCalculate4BodyProps().phaseSpaceFactor() <= 0.0){
      cout << "Warning in tryFlat4EventWithPhaseSpace " 
	   << " made 'good' event with bad phase space"
	   << " for sqrt(s123) " << sqrt(s123)
	   << " , sqrt(s12) " << sqrt(s12)
	   << " , sqrt(s12) + m3 " << sqrt(s12) + _pat[3].mass()
	   << " , sqrt(s123) + m4 " << sqrt(s123) + _pat[4].mass()
	   << " and m(D) " << _pat[0].mass()
	   << endl;
      cout << " compare " << s123 << ", " << thisEvent->t(4,0)
	   << " compare " << s12  << ", " << thisEvent->s(1,2)
	   << endl;
      return nullEvtPtr;
    }
    thisEvent->setWeight(w);
    returnEvent = thisEvent;
  }
  if(dbThis)cout << " got new event with weight " << returnEvent->getWeight() << endl;
  return returnEvent;
}

counted_ptr<DalitzEvent> DalitzBWArea::try3EventWithPhaseSpace(const Permutation& mapping) const{
  double maxWeight;
  return try3EventWithPhaseSpace(maxWeight, mapping);
}
counted_ptr<DalitzEvent> DalitzBWArea::try3EventWithPhaseSpace(double& maxWeight
							       , const Permutation& mapping) const{

  bool dbThis=false;

  if(dbThis){
    cout << " trying to make event with phase space for "
	 << *this
	 << endl;
  }  
  static TGenPhaseSpaceWithRnd gps;
  gps.setRandom(_rnd); // should not be necessary to repeat ch time, but no time to check and make sure

  TLorentzVector mumsP4;
  mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());

  static vector<TLorentzVector> p4_final(4), p4_finalMapped(4);
  static TLorentzVector p4_inter[1];
  static DalitzEventPattern patMapped;

  counted_ptr<DalitzEvent> nullEvtPtr(0);
  counted_ptr<DalitzEvent> returnEvent(0);

  // all set up such that we know that only
  // s12 can be filled;

  maxWeight = 1.0;

  double s12     = sf(1,2).second->generate(_rnd);
  if(s12 < 0) return nullEvtPtr;
  if(sqrt(s12) < _pat[1].mass() + _pat[2].mass()) return nullEvtPtr;

  if(sqrt(s12) + _pat[3].mass() > _pat[0].mass())return nullEvtPtr;
  
  if(dbThis){
    cout << "made s12 " << s12 << endl;
  }
  double s12Min  = sf(1,2).second->getCoordinate().min();
  double s12Max  = sf(1,2).second->getCoordinate().max();

  Double_t dgt[2];

  // D->s12, m3
  
  dgt[0] = sqrt(s12);
  dgt[1] = _pat[3].mass();
  mumsP4.SetXYZM(0, 0, 0, _pat[0].mass());
  p4_final[0] = mumsP4;
    
  if(! gps.SetDecay(mumsP4, 2, dgt)) return nullEvtPtr;
  if(dbThis){
    cout << " set decay to " << mumsP4 << " -> " 
	 << dgt[0] << ", " << dgt[1]
	 << endl;
  }
  gps.Generate();
  /* Note:
     usually, gps.Generate() produces weighted events, and
     to get un-weighted ones (i.e.events with weight 1) one
     needs to do:
     while(rnd->Rndm() > gps.Generate());
     However, 2-body decays all have weight 1.
     
  */
  p4_inter[0] = *(gps.GetDecay(0));
  p4_final[3] = *(gps.GetDecay(1));
    
  double ps = phaseSpaceIntegral2body(_pat[0].mass()
				      , sqrt(s12)
				      , _pat[3].mass());
  maxWeight *= phaseSpaceIntegral2body(_pat[0].mass()
				       ,sqrt(s12Min)
				       ,_pat[3].mass());
  
  // s12 -> m1, m2
  dgt[0] = _pat[1].mass();
  dgt[1] = _pat[2].mass();
  if(! gps.SetDecay(p4_inter[0], 2, dgt)) return nullEvtPtr;
  if(dbThis){
    cout << " set decay to " << p4_inter[0] << " -> " 
	 << dgt[0] << ", " << dgt[1]
	 << endl;
  }
  gps.Generate();
  p4_final[1] = *(gps.GetDecay(0));
  p4_final[2] = *(gps.GetDecay(1));
  
  ps *= phaseSpaceIntegral2body(sqrt(s12)
				, _pat[1].mass()
				, _pat[2].mass());
  maxWeight *= phaseSpaceIntegral2body(sqrt(s12Max)
				       , _pat[1].mass()
				       , _pat[2].mass());
  
  mapP4(p4_final, mapping, p4_finalMapped);
  mapping.mapOrder(_pat, patMapped);
  counted_ptr<DalitzEvent> thisEvent(new DalitzEvent(patMapped
						     , p4_finalMapped));
  if(thisEvent->phaseSpace() <= 0.0){
    cout << "WARNING in try3EventWithPhaseSpace: "
	 << " made 'good' event with bad phase space"
	 << " , sqrt(s12) " << sqrt(s12)
	 << " , sqrt(s12) + m3 " << sqrt(s12) + _pat[3].mass()
	 << " and m(D) " << _pat[0].mass()
	 << endl;
    cout << " compare " << s12  << ", " << thisEvent->s(1,2)
	 << endl;
    return nullEvtPtr;
  }
  thisEvent->setWeight(ps);
  returnEvent = thisEvent;
  if(dbThis){
    cout << " got new event with weight " 
	 << returnEvent->getWeight() << endl;
  }
  return returnEvent;
}

void DalitzBWArea::print(std::ostream& os) const{
  os << "Area size: " << size() << endl;
  for(std::map<DalitzCoordKey, std::pair<DalitzCoordinate, counted_ptr<IGenFct> > >::const_iterator
	it =_coords.begin();
      it != _coords.end();
      it++){
      
    os << "\n " << it->second.first;
  }
}


std::ostream& operator<<(std::ostream& os, const DalitzBWArea& da){
  da.print(os);
  return os;
}
//
