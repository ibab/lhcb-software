// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/MappedDalitzBWArea.h"
#include "Mint/Permutator.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace MINT;
/*
                                  _mapping
  'standard' Dalitz variables    ----------->  Dalitz variables where the
                                               box-limits have consecutive
					       indices:
					       t01, s12, s23, s34, t40
					       so that I can use
					       the usual
					       Calculate4BodyProbs etc
				 <-----------
        	               _inverseMapping

*/

MappedDalitzBWArea::MappedDalitzBWArea()
  : _rnd(0)
  , _pat()
  , _mappedPat()
  , _area()
  , _limits()
  , _mapping()
  , _inverseMapping()
{
  setup();
}
MappedDalitzBWArea::MappedDalitzBWArea(const DalitzEventPattern& pat
				   , TRandom* rnd)
  : _rnd(rnd)
  , _pat(pat)
  , _mappedPat(pat)
  , _area(pat)
  , _limits()
  , _mapping(pat.size())
  , _inverseMapping(pat.size())
{
  setup();
}
MappedDalitzBWArea::MappedDalitzBWArea(const DalitzEventPattern& pat
				   , const counted_ptr<IGenFct>& oneLimit
				   , TRandom* rnd
				   )
  : _rnd(rnd)
  , _pat(pat)
  , _mappedPat(pat)
  , _limits(1)
  , _mapping(pat.size())
  , _inverseMapping(pat.size())
{
  _limits[0] = oneLimit;
  setup();
}
MappedDalitzBWArea::MappedDalitzBWArea(const DalitzEventPattern& pat
				   , const counted_ptr<IGenFct>& oneLimit
				   , const counted_ptr<IGenFct>& otherLimit
				   , TRandom* rnd
				   )
  : _rnd(rnd)
  , _pat(pat)
  , _mappedPat(pat)
  , _limits(2)
  , _mapping(pat.size())
  , _inverseMapping(pat.size())
{
  _limits[0] = oneLimit;
  _limits[1] = otherLimit;
  /*
  cout << " just made a MappedDalitzBWArea with limits :"
       << oneLimit << ", " << otherLimit << " or "
       << _limits[0] << ", " << _limits[1] << endl;
  */
  setup();
  /*
  cout << " ..after setup: "
       << _limits[0] << ", " << _limits[1] << endl;
  */
}

MappedDalitzBWArea::MappedDalitzBWArea(const DalitzEventPattern& pat
				   , const std::vector<counted_ptr<IGenFct> >& lim
				   , TRandom* rnd
				   )
  : _rnd(rnd)
  , _pat(pat)
  , _mappedPat(pat)
  , _limits(lim)
  , _mapping(pat.size())
  , _inverseMapping(pat.size())
{
  setup();
}

MappedDalitzBWArea::MappedDalitzBWArea(const MappedDalitzBWArea& other)
  : _rnd(other._rnd)
  , _pat(other._pat)
  , _mappedPat(other._mappedPat)
  , _area(other._area)
  , _limits(other._limits)
  , _mapping(other._mapping)
  , _inverseMapping(other._inverseMapping)
{
}

MappedDalitzBWArea& MappedDalitzBWArea::operator=(const MappedDalitzBWArea& other){
  _rnd = other._rnd;
  _pat = other._pat;
  _mappedPat = other._mappedPat;
  _area = other._area;
  _limits = other._limits;
  _mapping = other._mapping;
  _inverseMapping = other._inverseMapping;
  return *this;
}

MappedDalitzBWArea::~MappedDalitzBWArea(){
}

bool MappedDalitzBWArea::allConsecutive() const{
  for(unsigned int i=0; i < _limits.size(); i++){
    DalitzCoordinate newCoordinates = (_limits[i])->getCoordinate().mapMe(_mapping);
    /*
    cout << "mapped new coordinates: " 
	 << i << ": " << newCoordinates << endl;
    */
    if(! (newCoordinates.I_am_Consecutive())) return false;
  }
  return true;
}

bool MappedDalitzBWArea::allStandardised() const{
  // such that only s123, s12,
  // or             s12, s34
  // are filled (or s123, or s12 for single 
  // limits).
  // Helps with generating later.

  if(_limits.size() > 2 || _limits.empty() ){
    return true; // no standards for that, all OK
  }
  DalitzCoordinate s123(1,2,3);
  DalitzCoordinate s12(1,2);
  DalitzCoordinate s34(3,4);
  if(_limits.size() == 1){
    if(_limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s123)) return true;
    if(_limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s12)) return true;
  }
  if(_limits.size() == 2){
    if(_limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s123)
       && _limits[1]->getCoordinate().mapMe(_mapping).sameIndices(s12)) return true;
    if(_limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s12)
       && _limits[1]->getCoordinate().mapMe(_mapping).sameIndices(s34)) return true;
    if(_limits[1]->getCoordinate().mapMe(_mapping).sameIndices(s123)
       && _limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s12)) return true;
    if(_limits[1]->getCoordinate().mapMe(_mapping).sameIndices(s12)
       && _limits[0]->getCoordinate().mapMe(_mapping).sameIndices(s34)) return true;
  }
  return false;
}

void MappedDalitzBWArea::findMapping(){
  bool dbThis=false;
  _mapping.makeUnity();
  if(allConsecutive() && allStandardised()) return;

  DalitzEventPattern tmpPat(_pat);
  tmpPat[0] =1;
  for(unsigned int i=1; i< _pat.size(); i++){
    tmpPat[i]=0;
  }

  Permutator ptor(tmpPat);// this finds all mappings
  // leaving the 0th particle alone (the mother) and
  // swapping all the daughters - the pattern
  // is set up to achieve this ('particles' 1,2,3,4,.. are identical)

  bool foundOne=false;
  for(unsigned int i=0; i< ptor.size(); i++){
    _mapping = ptor[i];
    if(dbThis) cout << " trying out mapping\n " << _mapping << endl;
    if(allConsecutive() && allStandardised()){
      foundOne=true;
      break;
    }
  }
  if(! foundOne){
    cout << " MappedDalitzBWArea::findMapping: WARNING!!"
	 << " didn't find a mapping!!!"
	 << endl;
    throw "shit!";
  }
  _inverseMapping = _mapping.getInverse();
  for(unsigned int i=0; i < _pat.size(); i++){
    int mappedIndex = _mapping[i];
    if(mappedIndex < 0 || mappedIndex >= (int) _mappedPat.size()){
      cout << "ERROR in MappedDalitzBWArea::findMapping()"
	   << "\n   Index out of range: " << mappedIndex
	   << " not in [0, " << _mappedPat.size()-1
	   << endl;
      throw "index out of range.";
    }
    if(dbThis) {
      cout << " mapped index of " << i 
	   << " = " << mappedIndex << endl;
    }
    _mappedPat[mappedIndex] = _pat[i];
  }

  if(dbThis){
    cout << " using the following mapping:\n "
	 << _mapping << endl;
    cout << "_mappedPat " << _mappedPat << endl;
  }
  return;
}

void MappedDalitzBWArea::setup(){
  bool dbThis=false;
  sort(_limits.begin(), _limits.end());

  findMapping();
  if(dbThis) cout << " mappedPat " << _mappedPat << endl;
  _area.setPattern(_mappedPat);
  _area.setRnd(_rnd);
  applyLimits();
}

void MappedDalitzBWArea::applyLimits(){
  bool dbThis=false;
  for(unsigned int i=0; i < _limits.size(); i++){
    if(dbThis){
      cout << "mapped co-ordinates " 
	   << (_limits[i])->getCoordinate().mapMe(_mapping)
	   << endl;
    }
    _area.setFcn((_limits[i])->getCoordinate().mapMe(_mapping), _limits[i]);
  }
}

/*
vector<TLorentzVector> MappedDalitzBWArea::mapP4(const DalitzEvent& evt
						  , const Permutation& mapping
						  ){
  vector<TLorentzVector> p4(evt.eventPattern().size());
  return mapP4(evt, mapping, p4);
}

std::vector<TLorentzVector>& 
MappedDalitzBWArea::mapP4(const DalitzEvent& evt
			  , const Permutation& mapping
			  , std::vector<TLorentzVector>& p4
			  ){

  unsigned int n = evt.eventPattern().size();
  p4.resize(n);

  for(unsigned int i=0; i < n; i++){
    int mappedIndex = mapping[i];
    if(mappedIndex < 0 || mappedIndex + 1 > (int) n){
      cout << "ERROR in MappedDalitzBWArea::mapP4()"
	   << "\n   Index out of range: " << mappedIndex
	   << " not in [0, " <<   n-1
	   << endl;
      throw "index out of range.";
    }
    p4[mappedIndex] = evt.p(i);
  }
  return p4;
}
*/
counted_ptr<DalitzEvent> MappedDalitzBWArea::tryEventForOwner() const{
  bool dbThis=false;
  if(dbThis){ 
    cout << "Hello from MappedDalitzBWArea::tryEventForOwner()" 
	 << endl;
  }
  counted_ptr<DalitzEvent> evt( _area.tryEventForOwner(_inverseMapping));

  /*
  if(dbThis && 0 != evt){
    counted_ptr<DalitzEvent> mappedEvt( _area.tryEventForOwner());
    if(0 == mappedEvt){
      return mappedEvt;
    }
    static vector<TLorentzVector> p4;
    mapP4( *mappedEvt, _inverseMapping, p4);
    
    counted_ptr<DalitzEvent> evtCheck(new DalitzEvent(_pat, p4));
    evtCheck->setWeight(mappedEvt->getWeight());

    cout << "compare new, old event and weight: "
	 << "\n\t new: " << *evt
	 << "\n\t old: " << *evtCheck
	 << endl;
  }
  */
  return evt;
}

double MappedDalitzBWArea::genValue(const DalitzEvent& evt) const{
  bool dbThis=false;

  double returnVal = _area.genValue(&evt, _inverseMapping);
  if(dbThis){
    vector<TLorentzVector> p4 =  mapP4(evt, _mapping );
    DalitzEvent mappedEvt(_mappedPat, p4);
    isInside(evt);
    cout << " mapping: " << _mapping << endl;    
    cout << "old style: " << _area.genValue(&mappedEvt)
	 << "\n new style: " << _area.genValue(&evt, _inverseMapping)
	 << "\n MappedDalitzBWArea::genValue returning " << returnVal << endl;
  }
  return returnVal;
}

bool MappedDalitzBWArea::isInside(const DalitzEvent& evt) const{
  bool dbThis=false;

  if(dbThis){
    vector<TLorentzVector> p4 = 
      mapP4(evt, _mapping );
    DalitzEvent mappedEvt(_mappedPat, p4);
    cout << " old style: " <<  _area.isInside(mappedEvt);
    cout << " new style: " << _area.isInside(evt, _inverseMapping);
  }
  return _area.isInside(evt, _inverseMapping);
}

bool MappedDalitzBWArea::isInside(const DalitzCoordinate& dc) const{
  return _area.isInside(dc.mapMe(_mapping));
}
bool MappedDalitzBWArea::isInside(const std::vector<DalitzCoordinate>& dcList) const{
  //  cout << " inside this: " << *this << endl;
  for(unsigned int i=0; i < dcList.size(); i++){
    if(! isInside(dcList[i])){
      //     cout << "Not inside: " << dcList[i]
      //	   << "\n\tmapped: " << dcList[i].mapMe(_mapping)
      //	   << endl;
      return false;
    }
  }
  return true;

}

void MappedDalitzBWArea::print(std::ostream& os) const{
  os << " MappedDalitzBWArea "
     << " limits: ";
  for(unsigned int i=0; i < _limits.size(); i++){
    if(0 == _limits[i]){
      os << "\n  ZERO-PTR LIMIT!" << endl;
    }else{
      os << "\n  " << (_limits[i])->getCoordinate() << endl;
    }
  }

  os << "\n  Mapping:\n " << _mapping
     << "\n area: " << _area;
}

bool MappedDalitzBWArea::setRnd(TRandom* rnd){
  _rnd = rnd;
  _area.setRnd(_rnd);
  return true;
}
std::ostream& operator<<(std::ostream& os, const MappedDalitzBWArea& mda){
  mda.print(os);
  return os;
}
//
