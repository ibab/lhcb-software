// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/MappedDalitzArea.h"
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

MappedDalitzArea::MappedDalitzArea()
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
MappedDalitzArea::MappedDalitzArea(const DalitzEventPattern& pat
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
MappedDalitzArea::MappedDalitzArea(const DalitzEventPattern& pat
				   , const DalitzCoordinate& oneLimit
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
MappedDalitzArea::MappedDalitzArea(const DalitzEventPattern& pat
				   , const DalitzCoordinate& oneLimit
				   , const DalitzCoordinate& otherLimit
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
  cout << " just made a MappedDalitzArea with limits :"
       << oneLimit << ", " << otherLimit << " or "
       << _limits[0] << ", " << _limits[1] << endl;
  */
  setup();
  /*
  cout << " ..after setup: "
       << _limits[0] << ", " << _limits[1] << endl;
  */
}

MappedDalitzArea::MappedDalitzArea(const DalitzEventPattern& pat
				   , const std::vector<DalitzCoordinate>& lim
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

MappedDalitzArea::MappedDalitzArea(const MappedDalitzArea& other)
  : _rnd(other._rnd)
  , _pat(other._pat)
  , _mappedPat(other._mappedPat)
  , _area(other._area)
  , _limits(other._limits)
  , _mapping(other._mapping)
  , _inverseMapping(other._inverseMapping)
{
}

MappedDalitzArea& MappedDalitzArea::operator=(const MappedDalitzArea& other){
  _rnd = other._rnd;
  _pat = other._pat;
  _mappedPat = other._mappedPat;
  _area = other._area;
  _limits = other._limits;
  _mapping = other._mapping;
  _inverseMapping = other._inverseMapping;
  return *this;
}

MappedDalitzArea::~MappedDalitzArea(){
}

bool MappedDalitzArea::allConsecutive() const{
  for(unsigned int i=0; i < _limits.size(); i++){
    DalitzCoordinate newCoordinates = (_limits[i]).mapMe(_mapping);
    /*
    cout << "mapped new coordinates: " 
	 << i << ": " << newCoordinates << endl;
    */
    if(! (newCoordinates.I_am_Consecutive())) return false;
  }
  return true;
}

void MappedDalitzArea::findMapping(){
  _mapping.makeUnity();
  if(allConsecutive()) return;

  DalitzEventPattern tmpPat(_pat);
  tmpPat[0] =1;
  for(unsigned int i=1; i< _pat.size(); i++){
    tmpPat[i]=0;
  }

  Permutator ptor(tmpPat);// this finds all mappings
  // leaving the 0th particle alone (the mother) and
  // swapping all the daughters - the pattern
  // is set up to achieve this ('particles' 1,2,3,4,.. are identical)

  for(unsigned int i=0; i< ptor.size(); i++){
    _mapping = ptor[i];
    cout << " trying out mapping\n " << _mapping << endl;
    if(allConsecutive()){
      break;
    }
  }
  _inverseMapping = _mapping.getInverse();
  for(unsigned int i=0; i < _pat.size(); i++){
    int mappedIndex = _mapping[i];
    if(mappedIndex < 0 || mappedIndex >= (int) _mappedPat.size()){
      cout << "ERROR in MappedDalitzArea::findMapping()"
	   << "\n   Index out of range: " << mappedIndex
	   << " not in [0, " << _mappedPat.size()-1
	   << endl;
      throw "index out of range.";
    }
    cout << " mapped index of " << i << " = " << mappedIndex << endl;
    _mappedPat[mappedIndex] = _pat[i];
  }

  cout << " using the following mapping:\n "
       << _mapping << endl;
  cout << "_mappedPat " << _mappedPat << endl;
}

void MappedDalitzArea::setup(){
  sort(_limits.begin(), _limits.end());

  findMapping();
  cout << " mappedPat " << _mappedPat << endl;
  _area.setPattern(_mappedPat);
  _area.setRnd(_rnd);
  applyLimits();
}

void MappedDalitzArea::applyLimits(){

  for(unsigned int i=0; i < _limits.size(); i++){
    cout << "mapped co-ordinates " << _limits[i].mapMe(_mapping) << endl;
    _area.setCoordinate(_limits[i].mapMe(_mapping));
  }
  _area.encloseInPhaseSpaceArea();
}

vector<TLorentzVector> MappedDalitzArea::mapP4(const DalitzEvent& evt
					       , const Permutation& mapping
					       ){
  vector<TLorentzVector> p4(evt.eventPattern().size());
  return mapP4(evt, mapping, p4);
}

vector<TLorentzVector>& MappedDalitzArea::mapP4(const DalitzEvent& evt
						, const Permutation& mapping
						, vector<TLorentzVector>& p4
						){
  
  unsigned int n = evt.eventPattern().size();
  p4.resize(n);
  
  for(unsigned int i=0; i < n; i++){
    int mappedIndex = mapping[i];
    if(mappedIndex < 0 || mappedIndex + 1 > (int) n){
      cout << "ERROR in MappedDalitzArea::mapP4()"
	   << "\n   Index out of range: " << mappedIndex
	   << " not in [0, " <<   n-1
	   << endl;
      throw "index out of range.";
    }
    p4[mappedIndex] = evt.p(i);
  }
  return p4;
}

counted_ptr<DalitzEvent> MappedDalitzArea::makeEventForOwner() const{
  counted_ptr<DalitzEvent> mappedEvt( _area.makeEventForOwner());
  if(0 == mappedEvt){
    return mappedEvt;
  }
  static vector<TLorentzVector> p4; 
  mapP4( *mappedEvt, _inverseMapping, p4 );
  
  counted_ptr<DalitzEvent> evt(new DalitzEvent(_pat, p4));
  return evt;
}

std::vector<DalitzCoordinate> MappedDalitzArea::centre() const{
  std::vector<DalitzCoordinate> c(_limits);
  for(unsigned int i=0; i < c.size(); i++){
    c[i].setVal(0.5*(c[i].min() + c[i].max()));
  }
  return c;
}
counted_ptr<DalitzEvent> MappedDalitzArea::makeEventForOwner(double scale0
							     , double scale1
							     , double scale2
							     , double scale3
							     , double scale4
							     ) const{
  counted_ptr<DalitzEvent> mappedEvt( _area.makeEventForOwner(scale0
							      , scale1
							      , scale2
							      , scale3
							      , scale4
							      )
				      );
  if(0 == mappedEvt){
    return mappedEvt;
  }
  vector<TLorentzVector> p4 = 
    mapP4( *mappedEvt, _inverseMapping );
  
  counted_ptr<DalitzEvent> evt(new DalitzEvent(_pat, p4));
  return evt;
}

bool MappedDalitzArea::isInside(const DalitzEvent& evt) const{

  vector<TLorentzVector> p4 = 
    mapP4(evt, _mapping );

  DalitzEvent mappedEvt(_mappedPat, p4);
  return _area.isInside(mappedEvt);
}

bool MappedDalitzArea::isInside(const DalitzCoordinate& dc) const{
  return _area.isInside(dc.mapMe(_mapping));
}
bool MappedDalitzArea::isInside(const std::vector<DalitzCoordinate>& dcList) const{
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


std::vector<MappedDalitzArea>
MappedDalitzArea::split(unsigned LimitIndex, unsigned int n) const{
  
  std::vector<MappedDalitzArea> newList;
  if(n <=0 || LimitIndex >= _limits.size()){
    newList.push_back(*this);
    return newList;
  }

  std::vector<DalitzCoordinate> splitLimits(_limits[LimitIndex].split(n));
  for(unsigned int i=0; i< splitLimits.size(); i++){
    MappedDalitzArea newArea(*this);
    newArea._limits[LimitIndex] = splitLimits[i];
    newArea.applyLimits();
    newList.push_back(newArea);
  }

  return newList;
}

std::vector<MappedDalitzArea>
MappedDalitzArea::split(unsigned int n) const{
  std::vector<MappedDalitzArea> returnList;
  if(n ==0 ) return returnList;

  std::vector<MappedDalitzArea> newList;
  returnList.push_back(*this);
  for(unsigned int limitIndex=0; limitIndex< _limits.size(); limitIndex++){
    std::vector<MappedDalitzArea> thisIterationsList;
    for(unsigned int j=0; j < returnList.size(); j++){
      std::vector<MappedDalitzArea> tempList = 
	returnList[j].split(limitIndex, n);
      thisIterationsList.insert(thisIterationsList.end()
				, tempList.begin(), tempList.end()
				);
    }
    returnList = thisIterationsList;
  }

  return returnList;
}

std::vector<MappedDalitzArea>
MappedDalitzArea::splitIfWiderThan(unsigned LimitIndex, double maxWidth) const{
  
  std::vector<MappedDalitzArea> newList;
  if(maxWidth <=0) return newList;
  if(LimitIndex >= _limits.size()) return newList;

  double width = 0.5*(_limits[LimitIndex].max() -  _limits[LimitIndex].min());
  // width in the sigma sense

  cout << " MappedDalitzArea::splitIfWiderThan: splitting "
       << _limits[LimitIndex]
       << " into...";
  unsigned int n = 2*((unsigned int) fabs(width/maxWidth)) + 1;
  cout << n << "  pieces." << endl;

  std::vector<DalitzCoordinate> splitLimits(_limits[LimitIndex].split(n));
  for(unsigned int i=0; i< splitLimits.size(); i++){
    MappedDalitzArea newArea(*this);
    newArea._limits[LimitIndex] = splitLimits[i];
    newArea.applyLimits();
    newList.push_back(newArea);
  }

  return newList;
}

std::vector<MappedDalitzArea>
MappedDalitzArea::splitIfWiderThan(double maxWidth) const{
  std::vector<MappedDalitzArea> returnList;
  if(maxWidth <= 0 ) return returnList;

  std::vector<MappedDalitzArea> newList;
  returnList.push_back(*this);
  for(unsigned int limitIndex=0; limitIndex< _limits.size(); limitIndex++){
    std::vector<MappedDalitzArea> thisIterationsList;
    for(unsigned int j=0; j < returnList.size(); j++){
      std::vector<MappedDalitzArea> tempList = 
	returnList[j].splitIfWiderThan(limitIndex, maxWidth);
      thisIterationsList.insert(thisIterationsList.end()
				, tempList.begin(), tempList.end()
				);
    }
    returnList = thisIterationsList;
  }

  return returnList;
}

DalitzCoordinate MappedDalitzArea::limit_s(const std::vector<int>& indices) const{
  DalitzCoordinate dc(indices);

  for(unsigned int i=0; i < _limits.size(); i++){
    if(_limits[i].size() != dc.size()) continue;
    for(unsigned int j=0; j < dc.size(); j++){
      if(_limits[i][j] != dc[j]){
	continue;
      }
    }
    return _limits[i];
  }
  return dc;
}
DalitzCoordinate MappedDalitzArea::limit_s(int i, int j, int k) const{
    std::vector<int> v;
    v.push_back(i);
    v.push_back(j);
    if(k >=0)v.push_back(k);
    return limit_s(v);
}

bool MappedDalitzArea::similar(const MappedDalitzArea& rhs) const{
  // doesn't work, don't know why.
  if(_limits.size() != rhs._limits.size()) return false;
  double epsilon = 1.e-4;
  for(unsigned int i=0; i < _limits.size(); i++){
    if(_limits[i].size() != rhs._limits[i].size()) return false;
    for(unsigned int j=0; j < _limits[i].size(); j++){
      if( (_limits[i])[j] != (rhs._limits[i])[j]) return false;
    }

    if( (_limits[i].min() - rhs._limits[i].min()) 
	> epsilon * (_limits[i].min() + rhs._limits[i].min()) 
	) return false;
    if( (_limits[i].max() - rhs._limits[i].max()) 
	> epsilon * (_limits[i].max() + rhs._limits[i].max()) 
	) return false;
  }
  return true;
}

bool MappedDalitzArea::less(const MappedDalitzArea& rhs) const{
  if(_limits.size() < rhs._limits.size()) return true;
  if(_limits.size() > rhs._limits.size()) return false;

  for(unsigned int i=0; i < _limits.size(); i++){
    if(_limits[i].size() < rhs._limits[i].size()) return true;
    if(_limits[i].size() > rhs._limits[i].size()) return false;
    for(unsigned int j=0; j < _limits[i].size(); j++){
      if( (_limits[i])[j] < (rhs._limits[i])[j]) return true;
      if( (_limits[i])[j] > (rhs._limits[i])[j]) return false;
    }

    if( (_limits[i].min() < rhs._limits[i].min()) ) return true;
    if( (_limits[i].min() > rhs._limits[i].min()) ) return false;

    if( (_limits[i].max() < rhs._limits[i].max()) ) return true;
    if( (_limits[i].max() > rhs._limits[i].max()) ) return false;
  }
  return false;
}

bool MappedDalitzArea::operator<(const MappedDalitzArea& rhs) const{
  return this->less(rhs);
}
bool MappedDalitzArea::operator<=(const MappedDalitzArea& rhs) const{
  return (this->less(rhs) || (*this) == rhs);
}
bool MappedDalitzArea::operator>(const MappedDalitzArea& rhs) const{
  return ! ((*this) < rhs || (*this) == rhs);
}
bool MappedDalitzArea::operator>=(const MappedDalitzArea& rhs) const{
  return ! ((*this) < rhs);
}

bool MappedDalitzArea::operator==(const MappedDalitzArea& rhs) const{
  if(_limits.size() != rhs._limits.size()) return false;
  for(unsigned int i=0; i < _limits.size(); i++){
    if(_limits[i] != rhs._limits[i]) return false;
  }
  return true;
}

bool MappedDalitzArea::operator!=(const MappedDalitzArea& rhs) const{
  return ! ( (*this) == rhs);
}


void MappedDalitzArea::print(std::ostream& os) const{
  os << " MappedDalitzArea "
     << " limits: ";
  for(unsigned int i=0; i < _limits.size(); i++){
    os << "\n  " << _limits[i] << endl;
  }

  os << "\n  Mapping:\n " << _mapping
     << "\n area: " << _area;
}

bool MappedDalitzArea::setRnd(TRandom* rnd){
  _rnd = rnd;
  _area.setRnd(rnd);
  return true;
}
std::ostream& operator<<(std::ostream& os, const MappedDalitzArea& mda){
  mda.print(os);
  return os;
}
//
