// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/DalitzBoxSet_Method2.h"
#include <iostream>

using namespace std;
using namespace MINT;

DalitzBoxSet_Method2::DalitzBoxSet_Method2(const DalitzBoxSet_Method2& other)
  : IEventGenerator<IDalitzEvent>()
  , DalitzBoxSet(other)
  , _pat(other._pat)
{
}

DalitzBoxSet_Method2::DalitzBoxSet_Method2(const DalitzEventPattern& pat
					   , TRandom* rnd)
  : DalitzBoxSet(rnd)
  , _pat(pat)
{
}

DalitzBoxSet_Method2::DalitzBoxSet_Method2(const DalitzBoxSet& boxSet
					   , const DalitzEventPattern& pat
					   )
  : DalitzBoxSet(boxSet)
  , _pat(pat)
{
  cout << " DalitzBoxSet_Method2 from DalitzBoxSet, and pattern " 
       << pat
       << ", ready? " << _ready
       << "; this is me:\n" << (*this)
       << endl;
  _ready = false;
}


void DalitzBoxSet_Method2::setBoxesToFlat(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].setFlatPhaseSpace();
  }
}

void DalitzBoxSet_Method2::getMaxHeight(){
  _maxHeight=-1;
  for(unsigned int i=0; i < this->size(); i++){
    double thisHeight = (*this)[i].height();
    if(thisHeight > _maxHeight) _maxHeight = thisHeight;
  }
}

void DalitzBoxSet_Method2::setEmptyBoxesHeight(){
  for(unsigned int i=0; i < this->size(); i++){
    double thisHeight = (*this)[i].height();
    if(thisHeight < 0){
       (*this)[i].setHeight(_maxHeight);
    }
  }
}

double DalitzBoxSet_Method2::maxHeight() const{
  return _maxHeight;
}

void DalitzBoxSet_Method2::getReady(){
  cout << " getting ready " << endl;
  setBoxesToFlat();
  DalitzBoxSet::getReady();
  getMaxHeight();
  setEmptyBoxesHeight();
  cout << "DalitzBoxSet_Method2: I am ready.\n" << (*this) << endl;
}

double DalitzBoxSet_Method2::eventsPDF(DalitzEvent& evt){

  if(0 == _amps ) return 0;
  double val = _amps->RealVal(evt);
  return val;
}

counted_ptr<DalitzEvent> DalitzBoxSet_Method2::tryWeightedEvent(){
  if(! _ready) getReady();
  int boxIndex = pickRandomVolume();
  //cout << "DalitzBoxSet::tryEvent() in volume " << boxIndex << endl;
  return (*this)[boxIndex].tryWeightedEventForOwner();
}

counted_ptr<IDalitzEvent> DalitzBoxSet_Method2::newEvent(){
  bool dbThis=false;
  if(dbThis)cout << "_ready = " << _ready << endl;
  if(dbThis)cout << " maxHeight " << maxHeight() << endl;
  if(maxHeight() < -9998 || ! _ready) getReady();
  if(dbThis)cout << " DalitzBoxSet_Method2::newEvent "
		 << " getting event " << endl;
  counted_ptr<DalitzEvent> ptr(0);
  while(0 == ptr){
    ptr = tryWeightedEvent();
  }
  if(dbThis) cout << "got event, returning " << ptr << endl;
  return ptr;
}

counted_ptr<DalitzEvent> DalitzBoxSet_Method2::tryEvent(){
  if(! _ready) getReady();
  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(_pat, _rnd));
  int boxIndex=-1;
  /*
  cout << "Hello from DalitzBoxSet_Method2::tryEvent()"
       << endl;
  cout << "I've got an event to try " << *evtPtr << endl;
  */
  for(unsigned int i=0; i< this->size(); i++){
    bool isIn = (*this)[i].insideArea(*evtPtr);
    /*
      cout << "Is event in box " << i << " ? " << endl;
      cout << "\t" << (isIn? "yes" : "no")
	   << endl;
    */
    if(isIn){
      boxIndex=i;
      break;
    }
  }
  if(boxIndex < 0 || boxIndex >= (int) this->size()){
    cout << "ERROR in DalitzBoxSet_Method2::tryEvent()"
	 << " boxIndex = " << boxIndex
	 << ", outside [" << 0 << ", " << this->size() << "]"
	 << endl;
    return counted_ptr<DalitzEvent>(0);
  }
  
  double thisHeight = (*this)[boxIndex].height();
  if(_rnd->Rndm()*maxHeight() >= thisHeight){
    return counted_ptr<DalitzEvent>(0);
  }
  // OK, so it passed the box stage, now internal:
  double pdf = eventsPDF(*evtPtr);
  if(pdf > thisHeight){
    cout << "ERROR, underestimated box height [" << boxIndex << "]"
	 << "\n\t\t thisHeight = " << thisHeight 
	 << "\n\t\t      < pdf = " << pdf
	 << "\n\t\t (maxHeight = " << maxHeight() << ")" << endl;
    (*this)[boxIndex].setHeight(3*pdf);
    getMaxHeight();
  }
  if(_rnd->Rndm()* thisHeight >= pdf) {
    return counted_ptr<DalitzEvent>(0);
  }

  return evtPtr;
}
