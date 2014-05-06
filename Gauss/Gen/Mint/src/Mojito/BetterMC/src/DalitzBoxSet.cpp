// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/DalitzBoxSet.h"
#include <algorithm>
#include <ctime>
//#include <algo.h>

using namespace std;
using namespace MINT;

class sameBoxByMappedArea{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    /*
    if(a.area() == b.area()){
      cout << " box a == box b "
	   << "\n  " << a
	   << "\n  " << b
	   << '\n'
	   << endl;
    }
    */
    return a.area() ==  b.area();
  }
};

class moreBoxByGuessedHeight{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.guessedHeight() >  b.guessedHeight();
  }
};
class lesserBoxByGuessedHeight{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.guessedHeight() <  b.guessedHeight();
  }
};

class lesserBoxByMappedArea{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.area() <  b.area();
  }
};

class sameBoxBySimilarMappedArea{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.area().similar(b.area());
  }
};

class lessBoxBySimilarMappedArea{
public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.area().less(b.area());
  }
};

class sortBoxByAreaSize{
 public:
  bool operator()(const DalitzBox& a, const DalitzBox& b){
    return a.area().size() < b.area().size();}
};

DalitzBoxSet::DalitzBoxSet(TRandom* rnd)
  : _ready(false)
{
  _rnd = rnd;
}

DalitzBoxSet::DalitzBoxSet(const DalitzBoxSet& other)
  : IEventGenerator<IDalitzEvent>()
  , std::vector<DalitzBox>(other)
  , _ready(other._ready)
  , _volumeProbs(other._volumeProbs)
  , _amps(other._amps)
  , _rnd(other._rnd)
{
  if(_ready) makeRelations();
}

void DalitzBoxSet::removeDuplicates(){

  //  lessBoxBySimilarMappedArea lesser;
  lesserBoxByMappedArea lesserArea;
  sameBoxByMappedArea same;
  moreBoxByGuessedHeight moreHeight;
  //sameBoxBySimilarMappedArea same;

  sort(this->begin(), this->end(), moreHeight);
  stable_sort(this->begin(), this->end(), lesserArea);

  std::vector<DalitzBox>::iterator 
    uniqueEnd = unique(this->begin(), this->end(), same);


  cout << "removeDuplicates: after sorting: " << endl;
  for(unsigned int i=0; i< this->size(); i++){
    bool equalToPrevious = false;
    if(i >= 1) equalToPrevious = same((*this)[i], (*this)[i-1]);
    cout << " " << i << ") "
	 << (*this)[i]
	 << "\n is equal to previous? "
	 << equalToPrevious
	 << '\n' << endl;
  }

  //  sameBoxBySimilarMappedArea same;
  /*
  this->erase( (unique(this->begin(), this->end(), same))
	       , this->end());
  */
  this->erase(uniqueEnd, this->end());

  cout << "removeDuplicates: after removing: " << endl;
  for(unsigned int i=0; i< this->size(); i++){
    cout << " " << i << ") "
	 << (*this)[i]
	 << '\n' << endl;
  }
}
void DalitzBoxSet::sortYourself(){
  cout << " DalitzBoxSet::sortYourself() " << endl;
    
  if(this->size() <= 1) return;
  cout << " creating sba" << endl;
  //sortBoxByAreaSize sba;
  moreBoxByGuessedHeight moreHeight;

  /*
  if(is_sorted(this->begin(), this->end(), sba)){
    return;
  }
  */
  stable_sort(this->begin(), this->end(), moreHeight);
  //  stable_sort(this->begin(), this->end(), sba);

  for(unsigned int i=0; i< this->size(); i++){
    (*this)[i].setNumber(i);
  }
  cout << " done sorting " << endl;
}

void DalitzBoxSet::makeRelations(){
  if(this->empty()) return;
  (*this)[0].setDaddy(0);

  if(this->size() <=1) return;
  //  sortYourself();
  for(unsigned int i=1; i< this->size(); i++){
    (*this)[i].setDaddy(&((*this)[i-1]));
  }
}

double DalitzBoxSet::VolumeSum() const{
  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    cout << " volume number " << i << ") " 
	 << (*this)[i].volume()
	 << endl;
    sum += (*this)[i].volume();
  }
  cout << " Volume sum: " << sum;
  return sum;
}

double DalitzBoxSet::AreaSum() const{
  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    sum += (*this)[i].area().size();
  }
  return sum;
}

void DalitzBoxSet::makeVolumeProbIntervals(){
  bool dbThis=false;
  if(this->empty()) return;
  // intervals each with a length proportional
  // to the volume of the corresponding box.
  // All put together add up to one.
  // For random number generation.

  _volumeProbs.clear();
  _volumeProbs.resize(this->size());

  double totalVolume = VolumeSum();
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].volume()/totalVolume;
    if(dbThis) cout << " volume probs [" << i <<"] = " << sum << endl;
    _volumeProbs[i] = sum;
  }
}

int DalitzBoxSet::pickRandomVolume(){
  if(! _ready) getReady();

  if(_volumeProbs.size() != this->size()){
    makeVolumeProbIntervals();
  }

  double rndNumber = _rnd->Rndm();
  for(unsigned int i=0; i< _volumeProbs.size(); i++){
    if(_volumeProbs[i] > rndNumber) return i;
  }
  cout << "WARNING in DalitzBoxSet::pickRandomVolume():"
       << " How odd - should never have gotten here."
       << " rndNumber = " << rndNumber
       << ", _volumeProbs[this->size()-1] = "
       << _volumeProbs[this->size()-1]
       << endl;
  return this->size()-1;
}

void DalitzBoxSet::add(const DalitzBox& box){
  _ready = false;
  cout << " DalitzBoxSet::addBox: adding box "  
       << " with " << box.eventList().size()
       << " events in it."
       << endl;
  this->push_back(box);
  cout << " done that." << endl;
}

void DalitzBoxSet::add(const std::vector<DalitzBox>& boxes){
  _ready = false;
  for(unsigned int i=0; i<boxes.size(); i++){
    this->push_back(boxes[i]);
  }
}

void DalitzBoxSet::add(const DalitzBoxSet& boxes){
  _ready = false;
  for(unsigned int i=0; i<boxes.size(); i++){
    this->push_back(boxes[i]);
  }
}

void DalitzBoxSet::setPDF(IGetRealEvent<IDalitzEvent>* amps){
  _ready = false;
  _amps = amps;
}

void DalitzBoxSet::setBoxPDFs(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].setAmps(_amps);
  }
}
void DalitzBoxSet::setBoxRnds(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].setRnd(_rnd);
  }
}
void DalitzBoxSet::getBoxesReady(){
  for(unsigned int i=0; i < this->size(); i++){
    cout << " getting box " << i << " ready." << endl;
    (*this)[i].getReady();
    cout << " got it." << endl;
  }
}

void DalitzBoxSet::callSortYourselfForDebug(){
  sortYourself();
}
void DalitzBoxSet::getReady(){
  time_t startTime = time(0);

  cout << "DalitzBoxSet::getReady()" << endl;
  cout << "before removing dupliates: " 
       << this->size() << " boxes." << endl;
  removeDuplicates();
  cout << "after removing dupliates: " 
       << this->size() << " boxes." << endl;
  sortYourself();
  cout << " sorted" << endl;
  makeRelations();
  cout << " made relations" << endl;
  cout << "I have the following " << this->size() << " boxes: " << endl;
  cout << *this << endl;

  setBoxPDFs();
  cout << " set the pdfs " << endl;
  setBoxRnds();
  cout << " set the rnds" << endl;
  getBoxesReady();
  cout << " got Boxes ready" << endl;
  makeVolumeProbIntervals();
  cout << " made the intervalse " << endl;

  double deltaT = difftime(time(0), startTime);
  cout << "DalitzBoxSet: after only:" << deltaT/60 << " min" << endl;
  cout << "DalitzBoxSet: I am ready." << endl;
  _ready = true;
}


DalitzBoxSet DalitzBoxSet::split(unsigned int nWays) const{
  if(nWays == 1) return (*this);
 
  DalitzBoxSet newSet;
  if(nWays == 0) return newSet;

  cout << " DalitzBoxSet: before splitting I have "
       << this->size() << " boxes"
       << endl;
  for(unsigned int i=0; i< this->size(); i++){
    newSet.add((*this)[i].split(nWays));
  }
  cout << "after, it's " << newSet.size() << endl;
  return newSet;
}

DalitzBoxSet DalitzBoxSet::splitIfWiderThan(double maxWidth) const{
 
  DalitzBoxSet newSet;

  cout << " DalitzBoxSet: before splitting I have "
       << this->size() << " boxes"
       << endl;
  for(unsigned int i=0; i< this->size(); i++){
    newSet.add((*this)[i].splitIfWiderThan(maxWidth));
  }
  cout << "after, it's " << newSet.size() << endl;
  return newSet;
}


counted_ptr<DalitzEvent> DalitzBoxSet::tryEvent(){
  if(! _ready) getReady();
  int boxIndex = pickRandomVolume();
  //cout << "DalitzBoxSet::tryEvent() in volume " << boxIndex << endl;
  return (*this)[boxIndex].tryEventForOwner();
}

counted_ptr<DalitzEvent> DalitzBoxSet::tryWeightedEvent(){
  if(! _ready) getReady();
  int boxIndex = pickRandomVolume();
  //cout << "DalitzBoxSet::tryEvent() in volume " << boxIndex << endl;
  return (*this)[boxIndex].tryWeightedEventForOwner();
}

counted_ptr<IDalitzEvent> DalitzBoxSet::newEvent(){
  bool dbThis = false;
  if(! _ready) getReady();
  if(dbThis)cout << " DalitzBoxSet::newEvent "
		 << " getting event " << endl;
  counted_ptr<DalitzEvent> ptr(0);
  while(0 == ptr){
    ptr = tryWeightedEvent();
  }
  if(dbThis) cout << "got event, returning " << ptr << endl;
  return ptr;
}

counted_ptr<DalitzEvent> DalitzBoxSet::generateEventForOwner(){
  counted_ptr<DalitzEvent> newEvent(0);
  unsigned int counter=0;
  unsigned int veryLargeNumber = 1000000000;
  do{
    newEvent = tryEvent();
    counter++;
  }while(0==newEvent && counter < veryLargeNumber);
  
  return newEvent;
}
DalitzEvent DalitzBoxSet::generateEvent(){
  counted_ptr<DalitzEvent> evtPtr = generateEventForOwner();
  if(0==evtPtr){
    cout << "WARNING: DalitzBoxSet::generateEvent()"
	 << " failed to generate event despite LARGE"
	 << " number of tries." << endl;
    DalitzEvent event;
  }
  DalitzEvent evt(*evtPtr);
  //  delete evtPtr;
  return evt;
}

bool DalitzBoxSet::setRnd(TRandom* rnd){
  _ready = false;
  if(0 == rnd) _rnd = gRandom;
  else _rnd = rnd; // this *should* do it, but for safety:
  for(unsigned int i=0; this->size(); i++){
    (*this)[i].setRnd(_rnd);
  }
  return true;
}
bool DalitzBoxSet::ensureFreshEvents(){
  _rnd->SetSeed(time(0)*4);
  setRnd(_rnd);
  return true;
}
void DalitzBoxSet::print(std::ostream& os) const{
  os << " DalitzBoxSet with " << this->size() << " boxes:"
     << "\n======================================";
  for(unsigned int i=0; i< this->size(); i++){
    os << "\n box " << i << "\n" << (*this)[i];
    os << "\n--------------------------------------";
  }
}
ostream& operator<<(ostream& os, const DalitzBoxSet& bs){
  bs.print(os);
  return os;
}
//
