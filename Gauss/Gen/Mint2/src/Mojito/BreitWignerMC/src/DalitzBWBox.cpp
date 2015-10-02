// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/DalitzBWBox.h"

#include "Mint/Minimiser.h"
#include "Mint/Minimisable.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/FitParameter.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;

DalitzBWBox::DalitzBWBox(TRandom* rnd) 
  : _name("noName")
  , _area()
  , _pat()
  , _amps(0)
  , _rnd(rnd)
  , _height(-1)
{}


DalitzBWBox::DalitzBWBox(const DalitzEventPattern& pat
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  : _name("noName")
  , _area(pat, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(1)
{
  setAmps(amps);
}

DalitzBWBox::DalitzBWBox(const DalitzEventPattern& pat
		     , const counted_ptr<IGenFct>& pdf
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  : _name("noName")
  , _area(pat, pdf, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(1)
{
  setAmps(amps);
}

DalitzBWBox::DalitzBWBox(const DalitzEventPattern& pat
		     , const std::vector<counted_ptr<IGenFct> >& limits
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  : _name("noName")
  , _area(pat, limits, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(-1)
{
  setAmps(amps);
}

DalitzBWBox::DalitzBWBox(const DalitzBWBox& other)
  : _name(other._name)
  , _area(other._area)
  , _pat(other._pat)
  , _amps(other._amps)
  , _rnd(other._rnd)
  , _height(other._height)
{
}

bool DalitzBWBox::setAmps(IReturnRealForEvent<IDalitzEvent>* amps){
  _amps = amps;
  if(0 == _amps) return false;
  return true;
}

DalitzBWBox::~DalitzBWBox(){
}

bool DalitzBWBox::insideArea(const DalitzEvent& evt) const{
  /*
  cout << "Hello from DalitzBWBox::insideArea()" << endl;
  cout << " my area is " << _area << endl;
  cout << ", or the same with (): " << area() << endl;
  cout << " and the event: " << evt << endl;
  cout << " Let's check if the event is inside the area: " << endl;
  */
  bool inside = area().isInside(evt);

  /*
  cout << "Goodbye from DalitzBWBox::insideArea(); returning " 
       << inside << endl;
  */

  return inside;
}

double DalitzBWBox::volume() const{
  //return height(); // 
  bool dbThis=false;
  if(dbThis){
    cout << " DalitzBWBox::volume() " << name() << ": "
	 << area().integral() << " * " << height() << endl;
  }
  return area().integral() * height();
}

double DalitzBWBox::genValue(const DalitzEvent& evt) const{
  bool dbThis=false;
  if(dbThis) {
    cout << "DalitzBWBox::genValue called " << endl;
  }

  double returnVal = area().genValue(evt)*height();
  if(dbThis) {
    cout << "DalitzBWBox::genValue returning "
	 <<  area().genValue(evt) << " * " << height() 
	 << " = "
	 << returnVal << endl;
  }
  return returnVal;
}

counted_ptr<DalitzEvent> DalitzBWBox::tryEventForOwner(){
  
  counted_ptr<DalitzEvent> evt(tryNewEvent());
  return evt;
}

counted_ptr<DalitzEvent> DalitzBWBox::makeEventForOwner(){
  int nTries;
  return makeEventForOwner(nTries);
}
counted_ptr<DalitzEvent> DalitzBWBox::makeEventForOwner(int& nTries){
  bool dbThis=false;

  int maxTries = 10000000;
  for(int i=0; i< maxTries; i++){
    counted_ptr<DalitzEvent> evt(tryNewEvent());
    if(dbThis && 0 != evt) cout << " DalitzBWBox::makeEventForOwner() got event " << evt << endl;
    if(0 != evt) {
      if(dbThis) cout << "DalitzBWBox::makeEventForOwner() - made event for value: "
		      << genValue(*evt) << endl;
      nTries = i+1;
      return evt;
    }
  }
  cout << "WARINING DalitzBWBox::makeEventForOwner() - no success after " << maxTries
       << " tries. Returning 0"
       << endl;
  nTries = maxTries;
  counted_ptr<DalitzEvent> evt(0);
  return evt;
}

counted_ptr<DalitzEvent> DalitzBWBox::tryNewEvent(){
  bool dbThis=false;
  counted_ptr<DalitzEvent> evt( area().tryEventForOwner() );
  if(dbThis && 0 != evt) {
    cout << " DalitzBWBox::tryNewEvent() got event with weight " 
	 << evt->getWeight() << endl;
  }
  return evt;
}

bool DalitzBWBox::setRnd(TRandom* rnd){
  _rnd = rnd;
  area().setRnd(_rnd);
  return true;
}
void DalitzBWBox::print(std::ostream& os) const{
  os << "DalitzBWBox: " << name()
     << "\n area " << area();
}

ostream& operator<<(ostream& os, const DalitzBWBox& box){
  box.print(os);
  return os;
}
//
