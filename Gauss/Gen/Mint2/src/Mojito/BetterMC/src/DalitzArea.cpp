// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/DalitzArea.h"

#include "Mint/DalitzEvent.h"
#include "Mint/Calculate4BodyProps.h"

#include "TRandom.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace MINT;

DalitzArea::DalitzArea()
  : _pat()
  , _rnd(gRandom)
  , _madeCMap(false)
  , _coords()
  , _t01(2,3,4)
  , _s12(1,2)
  , _s23(2,3)
  , _s34(3,4)
  , _t40(1,2,3)
{
  makeCoordinateMap();
}

DalitzArea::DalitzArea(const DalitzArea& other)
  : _pat(other._pat)
  , _rnd(other._rnd)
  , _madeCMap(false)
  , _coords()
  , _t01(other._t01)
  , _s12(other._s12)
  , _s23(other._s23)
  , _s34(other._s34)
  , _t40(other._t40)
{
  //   cout << "copying area" << endl;
  makeCoordinateMap();
  
  //  cout << " size before copy: " << other.size() << endl;
  //  cout << " size after copy: " << this->size() << endl;
  
}

DalitzArea& DalitzArea::operator=(const DalitzArea& other){
  _pat = other._pat;
  _rnd = other._rnd;
  _madeCMap = false;
  _coords.clear();
  _t01 = other._t01;
  _s12 = other._s12;
  _s23 = other._s23;
  _s34 = other._s34;
  _t40 = other._t40;
  makeCoordinateMap();
  return (*this);
}

DalitzArea::DalitzArea(const DalitzEventPattern& pat, TRandom* rnd)
  : _pat(pat)
  , _rnd(rnd)
  , _madeCMap(false)
  , _coords()
  , _t01(2,3,4)
  , _s12(1,2)
  , _s23(2,3)
  , _s34(3,4)
  , _t40(1,2,3)
{

  if(_pat.numDaughters() != 4
      && _pat.numDaughters() != 3
    ){
    cout << "DalitzArea::DalitzArea:"
	 << "\n  Sorry, can only deal with 3 and 4 body decays. "
	 << "\n  Please improve me so I can deal with decays like this: " 
	 << pat << endl;
    cout << "  I'll crash now." << endl;
    throw "DalitzArea: \"I'm no superman\"";
  }  
  makeCoordinateMap();
  makeMiMa();
}

void DalitzArea::setPattern(const  DalitzEventPattern& pat){
  _pat = pat;
  if(_pat.numDaughters() != 4
     && _pat.numDaughters() != 3
     ){
    cout << "DalitzArea::DalitzArea:"
	 << "\n  Sorry, can only deal with 3 or 4 body decays. "
	 << "\n  Please improve me so I can deal with decays like this: " 
	 << pat << endl;
    cout << "  I'll crash now." << endl;
    throw "DalitzArea: \"I'm no superman\"";
  }
  makeCoordinateMap();
  makeMiMa();
}

bool DalitzArea::setRnd(TRandom* rnd){
  _rnd = rnd;
  return true;
}

void DalitzArea::makeCoordinateMap() const{
  //cout << " Hi, this is DalitzArea::makeCoordinateMap()" << endl;
  _coords.clear();
  if(_pat.numDaughters() < 3) return;

  _coords[_s12] = &_s12;
  _coords[_s23] = &_s23;

  if(_pat.numDaughters() >= 4){
    _coords[_t01] = &_t01;
    _coords[_s34] = &_s34;
    _coords[_t40] = &_t40;

  }
  //cout << "DalitzArea::makeCoordinateMap() made map, returning true" << endl;
  _madeCMap = true;
}


void DalitzArea::makeMiMa(){
  double safetyFactor=1; // for debug only - otherwise 1
  //double safetyFactor=1.3;
  _s12.setMinMax( _pat.sijMin(1,2)/safetyFactor
		  , _pat.sijMax(1,2)*safetyFactor);
  _s23.setMinMax( _pat.sijMin(2,3)/safetyFactor
		  , _pat.sijMax(2,3)*safetyFactor);
  if(_pat.numDaughters() >= 4){
    _t01.setMinMax( _pat.sijMin(2,3,4)/safetyFactor
		    , _pat.sijMax(2,3,4)*safetyFactor);
    _s34.setMinMax( _pat.sijMin(3,4)/safetyFactor
		    , _pat.sijMax(3,4)*safetyFactor);
    _t40.setMinMax( _pat.sijMin(1,2,3)/safetyFactor
		    , _pat.sijMax(1,2,3)*safetyFactor);
  }

}

void DalitzArea::encloseInPhaseSpaceArea(double safetyFactor){
  if(! _madeCMap) makeCoordinateMap();
  for(map<DalitzCoordKey, DalitzCoordinate*>::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    double psMin = _pat.sijMin(it->first)/safetyFactor;
    double psMax = _pat.sijMax(it->first)*safetyFactor;

    if(it->second->min() < psMin){
      it->second->setMin(psMin);
    }
    if(it->second->max() > psMax){
      it->second->setMax(psMax);
    }
  }
  
}

void DalitzArea::setAllLimitsToPhaseSpaceArea(double safetyFactor){
  makeCoordinateMap();
  for(map<DalitzCoordKey, DalitzCoordinate*>::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    double psMin = _pat.sijMin(it->first)/safetyFactor;
    double psMax = _pat.sijMax(it->first)*safetyFactor;

    it->second->setMin(psMin);
    it->second->setMax(psMax);

  }
  
}



DalitzArea::~DalitzArea(){
}

bool DalitzArea::compatiblePattern(const DalitzEventPattern& p) const{
  if(_pat.size() != p.size()) return false;

  for(unsigned int i=0; i < _pat.size(); i++){
    if( abs(_pat[i]) != abs(p[i])) return false;
  }

  return true;
}

/*
bool DalitzArea::isInside(const DalitzEvent& evt) const{
  //cout << "Hello from  DalitzArea::isInside" << endl;
  if(! compatiblePattern(evt.eventPattern()){
    return false;
  }
  makeCoordinateMap();
  //cout << " Made Coordinate map " << endl;
  for(map<vector<int>, DalitzCoordinate*>::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    double val = evt.sij(it->first);
    if(val < it->second->min() || val >= it->second->max()) return false;
  }
  //cout << "returning true" << endl;
  return true;
}
*/

bool DalitzArea::isInside(const IDalitzEvent& evt) const{
  //cout << "Hello from  DalitzArea::isInside" << endl;
  if(! compatiblePattern(evt.eventPattern())){
    return false;
  }
  if(! _madeCMap) makeCoordinateMap();
  //cout << " Made Coordinate map " << endl;
  for(map<DalitzCoordKey, DalitzCoordinate* >::const_iterator 
	it = _coords.begin();
      it != _coords.end(); it++){
    double val = evt.sij(it->first);
    if(val < it->second->min() || val >= it->second->max()) return false;
  }
  //cout << "returning true" << endl;
  return true;
}
bool DalitzArea::isInside(const DalitzCoordinate& dc) const{

  if(! _madeCMap) makeCoordinateMap();
  //cout << " Made Coordinate map " << endl;
  double val = dc.val();
  if(_coords[dc]->min() <= val && _coords[dc]->max() > val){
    return true;
  }else{
    return false;
  }
}
bool DalitzArea::isInside(const std::vector<DalitzCoordinate>& dcList) const{
  for(unsigned int i=0; i < dcList.size(); i++){
    if(! isInside(dcList[i])){
      return false;
    }
  }
  return true;

}

double DalitzArea::size() const{
  if(_pat.size() < 3) return 0;

  double p=1;
  
  /*  cout << " DalitzArea::size(): returning..." << endl;
  cout << " e.g. _t01 " << _t01 << endl;
  cout << " e.g. _s12 " << _s12 << endl;
  cout << "_madeCMap = " << _madeCMap << endl;
  */
  makeCoordinateMap();
  for(map<vector<int>, DalitzCoordinate*>::const_iterator it = _coords.begin();
      it != _coords.end(); it++){
    //cout << "num indices " << it->second->size() << endl;
    //cout << " co-ordinate " << *(it->second) << endl;
    //cout << "( " << it->second->max() << " - " << it->second->min() << " ) * ";
    p *= (it->second->max() -  it->second->min());
  }
  //  cout << " = " << p << endl;
  return fabs(p);
}

counted_ptr<DalitzEvent> DalitzArea::makeEventForOwner( double scale0
							, double scale1
							, double scale2
							, double scale3
							, double scale4
							) const{

  double t01 = _t01.min() + scale0*(_t01.max() - _t01.min());
  double s12 = _s12.min() + scale1*(_s12.max() - _s12.min());
  double s23 = _s23.min() + scale2*(_s23.max() - _s23.min());
  double s34 = _s34.min() + scale3*(_s34.max() - _s34.min());
  double t40 = _t40.min() + scale4*(_t40.max() - _t40.min());

  Calculate4BodyProps c4b(t01, s12
			  , s23, s34
			  , t40
			  , _pat[0].mass()
			  , _pat[1].mass(), _pat[2].mass()
			  , _pat[3].mass(), _pat[4].mass()
			  );
  if(c4b.phaseSpaceFactor() <=0){
    if(false)cout << " for \n  " 
		  << t01 << "( " << _t01 << "), \n  "
		  << s12 << "( " << _s12 << "), \n  "
		  << s23 << "( " << _s23 << "), \n  "
		  << s34 << "( " << _s34 << "), \n  "
		  << t40 << "( " << _t40 << "), \n  "
		  << " outside phase space!! "
		  << endl;
    return counted_ptr<DalitzEvent>(0);
  }
  counted_ptr<DalitzEvent> evt(new DalitzEvent(_pat, t01, s12, s23, s34, t40));
  if(0 == evt){
    cout << "WARNING DalitzArea::make4Event()"
	 << " making the event failed at 'new DalitzEvent(...)'"
	 << " this shouldn't really happen."
	 << endl;
    return evt;
  }
  if(evt->phaseSpace() <= 0){
    return counted_ptr<DalitzEvent>(0);
  }
  
  return evt;
}

counted_ptr<DalitzEvent> DalitzArea::makeEventForOwner() const{
  if(_pat.numDaughters() == 3) return make3Event();
  if(_pat.numDaughters() == 4) return make4Event();
  cout << "ERROR in DalitzArea::makeEvent() can only make events"
       << " with 3 or 4 daughters. You want : " << _pat
       << endl;
  return counted_ptr<DalitzEvent>(0);
}

counted_ptr<DalitzEvent> DalitzArea::make3Event() const{

  cout << " ERROR DalitzArea::make3Event(): I don't work, yet. "
       << "\n    Please improve me. Sorry, will have to crash, now"
       << endl;
  throw "no making 3 body events in DalitzArea, yet";

  /*
  double s12 = _s12.min() + _rnd->Rndm()*(_s12.max() - _s12.min());
  double s23 = _s23.min() + _rnd->Rndm()*(_s23.max() - _s23.min());

  _evt = new DalitzEvent(_pat, s12, s23);
  if(_evt->phaseSpace() <= 0){
    deleteEvent();
    return 0;
  }
  
  return _evt;
  */
  return counted_ptr<DalitzEvent>(0);
}
counted_ptr<DalitzEvent> DalitzArea::make4Event() const{
  return makeEventForOwner( _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    );
}

bool DalitzArea::setCoordinate(const DalitzCoordinate& c){
  //cout << " size before setting c = " << c << " : " << size();
  map<vector<int>, DalitzCoordinate*>::iterator it = _coords.find(c);
  if(it == _coords.end()){
    cout << "ERROR: DalitzArea::setCoordinate failed to set coordinate "
	 << c << endl;
    return false;
  }
  *(it->second) = c;
  //cout << " size after: " << size() << endl;
  return true;
}

std::vector<DalitzArea> DalitzArea::split_in_all_dimensions(int n) const{
  std::vector<DalitzArea> v;
  if(n == 1){
    v.push_back(*this);
    return v;
  }
  if(_pat.numDaughters() == 3) return split_in_all_dim_3body(n);
  else if(_pat.numDaughters() == 4) return split_in_all_dim_4body(n);
  else{
    cout << "ERROR in DalitzArea::split_in_all_dimensions"
	 << " can't handle " << _pat.numDaughters() << " body decays."
	 << "\n\t Won't split, will return original box."
	 << endl;
    v.push_back(*this);
    return v;
  }
}

std::vector<DalitzArea> DalitzArea::split_in_all_dim_3body(int n) const{
  std::vector<DalitzArea> v;
  if(n <= 1){
    v.push_back(*this);
    return v;
  }
  std::vector<DalitzCoordinate> vs12 = _s12.split(n);
  std::vector<DalitzCoordinate> vs23 = _s23.split(n);
  
  for(unsigned int i12 = 0; i12 < vs12.size(); i12++){
    for(unsigned int i23 = 0; i23 < vs23.size(); i23++){
      
      DalitzArea na(*this);
      na.setCoordinate(vs12[i12]);
      na.setCoordinate(vs23[i23]);
      
      v.push_back(na);
    }
  }
  return v;
}

std::vector<DalitzArea> DalitzArea::split_in_all_dim_4body(int n) const{
  bool dbThis=false;

  std::vector<DalitzArea> v;
  if(n <= 1){
    v.push_back(*this);
    return v;
  }

  std::vector<DalitzCoordinate> vt01 = _t01.split(n);
  std::vector<DalitzCoordinate> vs12 = _s12.split(n);
  std::vector<DalitzCoordinate> vs23 = _s23.split(n);
  std::vector<DalitzCoordinate> vs34 = _s34.split(n);
  std::vector<DalitzCoordinate> vt40 = _t40.split(n);

  for(unsigned int i01 = 0; i01 < vt01.size(); i01++){
    for(unsigned int i12 = 0; i12 < vs12.size(); i12++){
      for(unsigned int i23 = 0; i23 < vs23.size(); i23++){
	for(unsigned int i34 = 0; i34 < vs34.size(); i34++){
	  for(unsigned int i40 = 0; i40 < vt40.size(); i40++){

	    DalitzArea na(*this);
	    na.setCoordinate(vt01[i01]);
	    na.setCoordinate(vs12[i12]);
	    na.setCoordinate(vs23[i23]);
	    na.setCoordinate(vs34[i34]);
	    na.setCoordinate(vt40[i40]);

	    v.push_back(na);
	  }
	}
      }
    }
  }
  if(dbThis){
    cout << "after splitting, n = " << v.size() << endl;
  }
  return v;
}


void DalitzArea::print(std::ostream& os) const{
  makeCoordinateMap();
  os << "Area size: " << size();
  for(std::map<std::vector<int>, DalitzCoordinate*>::const_iterator
	it =_coords.begin();
      it != _coords.end();
      it++){      
    os << "\n " << *(it->second);
  }
}


std::ostream& operator<<(std::ostream& os, const DalitzArea& da){
  da.print(os);
  return os;
}
//
