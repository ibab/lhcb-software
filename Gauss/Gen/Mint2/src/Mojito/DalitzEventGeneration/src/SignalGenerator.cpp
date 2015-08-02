// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/SignalGenerator.h"
#include <ctime>
#include <iostream>
#include <complex>

using namespace std;
using namespace MINT;

SignalGenerator::SignalGenerator(const DalitzEventPattern& pat
				 , TRandom* rnd
				 )
  : BaseGenerator(rnd)
  , _pat(pat)
  , _myOwnPSet()
  , _counted_amps(new FitAmpSum(pat, myMPS()))
  , _amps(_counted_amps.get())
  , _boxes(_amps->makeEventGenerator(pat))
{
  _boxes->setRnd(rnd);
}
SignalGenerator::SignalGenerator(const DalitzEventPattern& pat
				 , double rB
				 , double phase
				 , TRandom* rnd
				 )
  : BaseGenerator(rnd)
  , _pat(pat)
  , _myOwnPSet()
  , _counted_amps(0)
  , _amps(0)
  , _boxes(0)
{
  DalitzEventPattern cpPat(pat);
  cpPat[0].antiThis();

  counted_ptr<FitAmpSum> fs(new FitAmpSum(pat, myMPS()));
  FitAmpSum cpAmps(pat, myMPS());
  cpAmps *= polar(rB, phase);
  fs->add(cpAmps);
  _counted_amps = (counted_ptr<IFastAmplitudeIntegrable>) fs;
  _amps = _counted_amps.get();

  makeBoxes();

  /*
  counted_ptr< IUnweightedEventGenerator<IDalitzEvent> > 
    bpt(fs->makeEventGenerator());

  _boxes = bpt;
  _boxes->setRnd(_rnd);
  */
}
SignalGenerator::SignalGenerator(const DalitzEventPattern& pat
				 , IFastAmplitudeIntegrable* amps
				 , TRandom* rnd)
  : BaseGenerator(rnd)
  , _pat(pat)
  , _myOwnPSet()
  , _amps(amps)
  , _boxes(_amps->makeEventGenerator(pat))
{
  _boxes->setRnd(_rnd);
}

MINT::MinuitParameterSet* SignalGenerator::myMPS(){
  // seems like an unnecessary complication
  // but it's a "hook" to add the option to pass
  // other MinuitParameterSets to SignalGenerator
  // should this become necessary.
  return & _myOwnPSet;
}


bool SignalGenerator::makeBoxes(){
  if(0 == _amps) return 0;
  counted_ptr< IUnweightedEventGenerator<IDalitzEvent> > 
    bpt(_amps->makeEventGenerator(_pat));
  _boxes = bpt;
  if(0 == _boxes) return 0;
  _boxes->setRnd(_rnd);
  return true;
}

counted_ptr<IDalitzEvent> SignalGenerator::tryDalitzEvent(){
  bool dbThis=false;
  if(0 == _boxes) makeBoxes();
  if(_unWeighted){
    counted_ptr<IDalitzEvent> evtPtr(_boxes->newUnweightedEvent());
    if(0 != evtPtr) evtPtr->setMothers3Momentum(mothers3Momentum());
    if(dbThis && 0 != evtPtr){
      cout << "SignalGenerator::tryDalitzEvent(): made un-weighted event "
	   << "with weight " << evtPtr->getWeight() << endl;
    }
    return evtPtr;
  }else{
    counted_ptr<IDalitzEvent> evtPtr(_boxes->newEvent());
    if(0 != evtPtr) evtPtr->setMothers3Momentum(mothers3Momentum());
    if(dbThis && 0 != evtPtr){
      cout << "SignalGenerator::tryDalitzEvent(): made weighted event "
	   << "with weight " << evtPtr->getWeight() << endl;
    }
    return evtPtr;
  }
}

counted_ptr<IDalitzEvent> SignalGenerator::newDalitzEvent(){
  bool dbThis=false;
  counted_ptr<IDalitzEvent> evtPtr(0);
  int counter(0);
  int largeNumber(1000000);

  do{
    evtPtr = tryDalitzEvent();
  }while(0 == evtPtr &&  counter++ < largeNumber);
  if(saveEvents()) _evtList->Add(evtPtr);
  if(dbThis){
    cout << "SignalGenerator::newDalitzEvent:"
	 << " just generated this event:\n" << *evtPtr << endl;
  }
  return evtPtr;
}

bool SignalGenerator::ensureFreshEvents(){
  _boxes->ensureFreshEvents();
  return BaseGenerator::ensureFreshEvents();
}
counted_ptr<IDalitzEvent> SignalGenerator::newEvent(){
  return counted_ptr<IDalitzEvent>(newDalitzEvent());
}

//
