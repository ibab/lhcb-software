#include "Mint/DalitzPhaseSpaceBox.h"
#include "Mint/phaseSpaceIntegrals.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;

DalitzPhaseSpaceBox::DalitzPhaseSpaceBox(TRandom* rnd) 
  : _name("noName")
  , _pat()
  , _rnd(rnd)
  , _height(1)
  , _weight(1)
  , _phaseSpaceIntegral(-1)
{}


DalitzPhaseSpaceBox::DalitzPhaseSpaceBox(const DalitzEventPattern& pat
		     , TRandom* rnd)
  : _name("noName")
  , _pat(pat)
  , _rnd(rnd)
  , _height(1)
  , _weight(1)
  , _phaseSpaceIntegral(-1)
{
  doPhaseSpaceIntegral();
}

DalitzPhaseSpaceBox::DalitzPhaseSpaceBox(const DalitzPhaseSpaceBox& other)
  : _name(other._name)
  , _pat(other._pat)
  , _rnd(other._rnd)
  , _height(other._height)
  , _weight(other._weight)
  , _phaseSpaceIntegral(other._phaseSpaceIntegral)
{
}

DalitzPhaseSpaceBox::~DalitzPhaseSpaceBox(){
}

void DalitzPhaseSpaceBox::setPattern(const DalitzEventPattern& pat){
  _pat = pat;
  doPhaseSpaceIntegral();
}
double DalitzPhaseSpaceBox::doPhaseSpaceIntegral(){
  _phaseSpaceIntegral = weight()*phaseSpaceIntegral_upTo4body(_pat);
  return _phaseSpaceIntegral;
}
double DalitzPhaseSpaceBox::phaseSpaceIntegral() const{
  return _phaseSpaceIntegral;
}
double DalitzPhaseSpaceBox::phaseSpaceIntegral(){
  if(_phaseSpaceIntegral < 0) doPhaseSpaceIntegral();
  return _phaseSpaceIntegral;
}

bool DalitzPhaseSpaceBox::insideArea(const DalitzEvent& evt) const{

  return (evt.phaseSpace() > 0);
}

double DalitzPhaseSpaceBox::volume() const{
  // bool dbThis = false;
  return phaseSpaceIntegral() * height();
}

double DalitzPhaseSpaceBox::genValue(const DalitzEvent& ) const{
  // returns generator value w/o phase space - constant, here.
  return genValue();
}
double DalitzPhaseSpaceBox::genValue() const{
  bool dbThis=false;
  if(dbThis) {
    cout << "DalitzPhaseSpaceBox::genValue called " << endl;
  }

  double returnVal = height() * weight();
  if(dbThis) {
    cout << "DalitzPhaseSpaceBox::genValue returning "
	 <<  weight() << " * " << height() 
	 << " = "
	 << returnVal << endl;
  }
  return returnVal;
}

counted_ptr<DalitzEvent> DalitzPhaseSpaceBox::tryEventForOwner(){
  
  counted_ptr<DalitzEvent> evt(tryNewEvent());
  return evt;
}

counted_ptr<DalitzEvent> DalitzPhaseSpaceBox::makeEventForOwner(){
  //  bool dbThis=false;
  if(_pat.numDaughters() < 2){
    counted_ptr<DalitzEvent> zero(0);
    return zero;
  }
  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(_pat, _rnd));
  evtPtr->setWeight(weight());
  // above constructor makes a phase space event, weight 1.
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

counted_ptr<DalitzEvent> DalitzPhaseSpaceBox::tryNewEvent(){
  return makeEventForOwner(); 
}

bool DalitzPhaseSpaceBox::setRnd(TRandom* rnd){
  _rnd = rnd;
  return true;
}
void DalitzPhaseSpaceBox::print(std::ostream& os) const{
  os << "DalitzPhaseSpaceBox: " << name();
}

bool DalitzPhaseSpaceBox::checkIntegration() const{
  cout << "DalitzPhaseSpaceBox: can't check integration, yet - improve me!" << endl;
  return true;
}

ostream& operator<<(ostream& os, const DalitzPhaseSpaceBox& box){
  box.print(os);
  return os;
}
//
