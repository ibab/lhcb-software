// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include "Mint/FlexiFastAmplitudeIntegrator.h"
#include "Mint/FitAmpSum.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/Minimiser.h"

#include "TRandom.h"
#include "TRandom3.h"

#include <iostream>
#include <sstream>

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace MINT;

FlexiFastAmplitudeIntegrator
::FlexiFastAmplitudeIntegrator( const DalitzEventPattern& pattern
				, IFastAmplitudeIntegrable* amps
				, IEventGenerator<IDalitzEvent> * generator
				, TRandom* rnd
				, double precision
				, const std::string& fastFlexiEventFileName
			   )
  : _db(false)
  , _fastFlexiEventList(fastFlexiEventFileName, "RECREATE")
{
  initialise(pattern, amps, generator, rnd, precision);
}

FlexiFastAmplitudeIntegrator::FlexiFastAmplitudeIntegrator(const std::string& fastFlexiEventFileName)
  : _initialised(0), _db(false), _fastFlexiEventList(fastFlexiEventFileName, "RECREATE")

{
}


bool FlexiFastAmplitudeIntegrator
::initialise(const DalitzEventPattern& pattern
	     , IFastAmplitudeIntegrable* amps
	     , IEventGenerator<IDalitzEvent> * generator
	     , TRandom* rnd
	     , double precision
	     ){
  bool sc=true;
  sc &= setValues(pattern, amps, generator, rnd, precision);
  sc &= generateEnoughEvents();
  return sc;
}


bool FlexiFastAmplitudeIntegrator::add(const FlexiFastAmplitudeIntegrator& other){
  int totalEvents = _numEvents + other._numEvents;
  _fastFlexiEventList.Add(other._fastFlexiEventList);

  if(totalEvents > 0){
    _mean = (_numEvents * _mean + other._numEvents * other._mean)/totalEvents;
  }
  _numEvents = totalEvents;
  _Ncalls   += other._Ncalls;
  if(0 != this->_integCalc){
    _integCalc->add(other._integCalc);
  }else{
    if(0 != other._integCalc){
      counted_ptr<IntegCalculator> nL(new IntegCalculator(*(other._integCalc)));
      _integCalc = nL;
    }
  }
  if(0 != this->_integCalc_copyForDebug){
    _integCalc_copyForDebug->add(other._integCalc_copyForDebug);
  }else{
    if(0 != other._integCalc_copyForDebug){
      counted_ptr<IntegCalculator> 
	nLc(new IntegCalculator(*(other._integCalc_copyForDebug)));
      _integCalc_copyForDebug = nLc;
    }
  }
  _initialised |= other._initialised;
  _db |= other._db;

  // other quantities we take over if they aren't defined, here:
  if(0 == _amps) _amps = other._amps;
  if(0 == _rnd) _rnd = other._rnd;
  if(0 == _localRnd) _localRnd = other._localRnd;
  if(_precision <= 0.0) _precision = other._precision;

  return true;
}

bool FlexiFastAmplitudeIntegrator::ensureFreshEvents(){
  _rnd = makeNewRnd(time(0));
  if(0 != _generator){
    _generator->ensureFreshEvents();
  }
  return true;
}

TRandom* FlexiFastAmplitudeIntegrator::makeNewRnd(int seed){
  if(seed < -9998) seed = time(0);
  cout << "FlexiFastAmplitudeIntegrator::makeNewRnd with seed " << seed << endl;
  counted_ptr<TRandom> lr(new TRandom3(seed));
  _localRnd = lr;
  _rnd = lr.get();
  return _rnd;
}
bool FlexiFastAmplitudeIntegrator
::setValues(const DalitzEventPattern& pattern
	    , IFastAmplitudeIntegrable* amps
	    //, counted_ptr<IGetDalitzEvent> weight
	    , IEventGenerator<IDalitzEvent> * generator
	    , TRandom* rnd
	    , double precision
	    ){
  _pat  = pattern;
  _amps = amps;
  //_weight = weight;
  if(0 != rnd){
    _rnd = rnd;
  }else{
    _rnd = makeNewRnd(time(0));
  }
  _precision = precision;
  
  _generator = generator;

  _integCalc = amps->makeIntegCalculator();

  if(_db) _integCalc_copyForDebug = amps->makeIntegCalculator();

  if(0 == _integCalc){
    cout << "ERROR in FlexiFastAmplitudeIntegrator::initialise"
	 << " amps->makeIntegCalculator()"
	 << " return NULL pointer!" << endl;
    cout << "\t This will go wrong." << endl;
  }
  _Ncalls=0;

  _initialised = true;
  return _initialised;
}

int FlexiFastAmplitudeIntegrator::updateEventSet(long int Nevents){
  if(! _initialised){
    cout << " FlexiFastAmplitudeIntegrator::updateEventSet "
	 << " need to know pattern first." << endl;
    return 0;
  }

  _integCalc->startIntegration();
  int missingEvents = Nevents - _integCalc->numEvents();

  if(missingEvents > 0){
    cout << "missing events: " << missingEvents 
	 << " = " << Nevents 
	 << " - " << _integCalc->numEvents()
	 << endl;
    addEvents(missingEvents);
  }else{
    cout << "want " << Nevents
	 << ", have " << _integCalc->numEvents()
	 << ", so no more events missing." << endl;
  }
  _integCalc->endIntegration();
  return _integCalc->numEvents();
}

void FlexiFastAmplitudeIntegrator::reIntegrate(){
  _integCalc->startReIntegration();
  for(unsigned int i=0; i < _fastFlexiEventList.size(); i++){
    reAddEvent(_fastFlexiEventList.getEvent(i));
  }
  _integCalc->endIntegration();
}

double FlexiFastAmplitudeIntegrator::weight(IDalitzEvent* ){
  return 1;
  // (not in use, but left as a hook should the need arise)
  /*
  if(0 == _weight) return 1;

  _weight->setEvent(evtPtr);
  double wght = _weight->RealVal();
  _weight->resetEventRecord();
  return wght;
  */
}

void FlexiFastAmplitudeIntegrator::addEvent(IDalitzEvent& evt){
  _fastFlexiEventList.Add(evt);
  _integCalc->addEvent(&evt, weight(&evt));
}
void FlexiFastAmplitudeIntegrator::reAddEvent(DalitzEvent evt){
  _integCalc->reAddEvent(&evt, weight(&evt));
}

int FlexiFastAmplitudeIntegrator::addEvents(long int Nevents){
  bool dbThis=false;
  if(dbThis) cout << "FlexiFastAmplitudeIntegrator::addEvents "
		  << " got called " << endl;
  if(Nevents <= 0) return _integCalc->numEvents();

  if(0 == _generator){
    cout << "FlexiFastAmplitudeIntegrator::addEvents "
	 << " adding flat events. " << endl;
    for(int i=0; i < Nevents; i++){
      DalitzEvent evt(_pat, _rnd);
      addEvent(evt);
    }
  }else{
    int N_success = 0;
    unsigned long int maxTries = (unsigned long int) Nevents * 1000;
    // dealing with possible overflow:
    if(maxTries < (unsigned long int) Nevents) maxTries = Nevents * 100;
    if(maxTries < (unsigned long int) Nevents) maxTries = Nevents * 10;
    if(maxTries < (unsigned long int) Nevents) maxTries = Nevents;

    time_t startTime = time(0);
    for(unsigned long int i=0; i < maxTries && N_success < Nevents; i++){
      counted_ptr<IDalitzEvent> ptr(_generator->newEvent());
      if(dbThis) cout << "got event with ptr: " << ptr << endl;
      if(0 == ptr){
	if(_generator->exhausted()){
	  cout << "ERROR in FlexiFastAmplitudeIntegrator::addEvents"
	       << "\n\t Generator exhausted, cannot generate more events."
	       << endl;
	  break;
	}
      }else{
	addEvent(*ptr);
	N_success++;
	int                    printEvery =   1000;
	if(N_success >  1000)  printEvery =  50000;
	if(N_success > 100000) printEvery = 100000;
	bool printout = (N_success%printEvery == 0);
	if(dbThis) printout |= (N_success < 20);

	if(printout){
	  cout << " FlexiFastAmplitudeIntegrator::addEvents: added " << N_success 
	       << " th event";
	  evaluateSum();
	  double v= variance();
	  double sigma = -9999;
	  if(v > 0) sigma = sqrt(v);
	  cout << "\t integ= " << _mean << " +/- " << sigma;
	  cout << "\t("
	       << MINT::stringtime(difftime(time(0), startTime))
	       << ")";
	  cout << endl;
	}
      }
    }
  }
  if(_integCalc->numEvents() <  Nevents){
    cout << "ERROR in FlexiFastAmplitudeIntegrator::addEvents"
	 << "\n\t> I was supposed to add " << Nevents << " events,"
	 << "\n\t> but now the total list size is only " 
	 << _integCalc->numEvents()
	 << endl;
  }else{
    cout << " added " << Nevents << " _integCalc->numEvents "
	 << _integCalc->numEvents() << endl;

  }
  return _integCalc->numEvents();
}

double FlexiFastAmplitudeIntegrator::variance() const{
  return _integCalc->variance();
}
double FlexiFastAmplitudeIntegrator::evaluateSum(){
  time_t tstart = time(0);
  _Ncalls++;

  _mean    = _integCalc->integral();
  double v = variance();
  double sigma = -9999;
  if(v >= 0) sigma = sqrt(v);
  double actualPrecision = -9999;
  if(_mean != 0 && sigma > 0) actualPrecision = sigma/_mean;

  double delT = difftime(time(0), tstart);

  int                  printEvery =    100;
  if(_Ncalls >   1000) printEvery =   1000;
  if(_Ncalls >  10000) printEvery =  10000;
  if(_Ncalls > 100000) printEvery = 100000;
  bool printout = _Ncalls < 3 || (0 == _Ncalls%printEvery);
 
  if(printout || _db){
    cout << " FlexiFastAmplitudeIntegrator::evaluateSum() for "
	 << _integCalc->numEvents() << " events, "
	 << _Ncalls << " th call:"
	 << "\n\t> getting: " << _mean << " +/- " << sigma
	 << "\n\t> precision: requested: " << _precision*100
	 << "%, actual: " << actualPrecision*100 << "%"
	 << "\n\t> This took " << delT << " s."
	 << endl;
    if(_db){
      cout << "checking save and read-back:" << endl;
      _integCalc->save("checkSaveAndReadBack");
      _integCalc_copyForDebug->retrieve("checkSaveAndReadBack");
      cout << " copy values:"
	   << "\n\t     mean: " << _integCalc_copyForDebug->integral()
	   << "\n\t variance: " << _integCalc_copyForDebug->variance()
	   << endl;
    }
  }
  return _mean;
}

int FlexiFastAmplitudeIntegrator::determineNumEvents(){
  updateEventSet(_minEvents);
  evaluateSum();
  double v = variance();
  double safetyFactor = 1.15;
  double maxVariance = _precision*_precision * _mean*_mean;
  _numEvents = _integCalc->numEvents() * 
    (v*safetyFactor/maxVariance);
  // round to the nearest 100:
  _numEvents = (_numEvents/100 + 1)*100;

  cout << " FlexiFastAmplitudeIntegrator::determineNumEvents():"
       << "\n\t> mean: " << _mean << ", rms " << sqrt(v)
       << "\n\t> currently, our relative precision is " 
       << sqrt(v)/_mean
       << "\n\t> to achieve the requested pecision of " << _precision << ","
       << "\n\t> (with a safety factor) I think we'll need " 
       << (v*safetyFactor/maxVariance)
       << " times that, i.e. " << _numEvents << " events" << endl;

  return _numEvents;
}

int FlexiFastAmplitudeIntegrator::generateEnoughEvents(){
  updateEventSet(_minEvents);
  int counter(0);
  while(determineNumEvents() > _integCalc->numEvents() &&
	counter < 10 && _numEvents > 0){
    updateEventSet(_numEvents);
    counter++;
  }
  return _integCalc->numEvents();
}

double FlexiFastAmplitudeIntegrator::getVal(){
  if( _integCalc->needToReIntegrate() ) reIntegrate();
  evaluateSum();
  return _mean;
}

DalitzHistoSet FlexiFastAmplitudeIntegrator::histoSet() const{
  return _integCalc->histoSet();
}
void FlexiFastAmplitudeIntegrator::saveEachAmpsHistograms(const std::string& prefix) const{
  return _integCalc->saveEachAmpsHistograms(prefix);
}
std::vector<DalitzHistoSet> FlexiFastAmplitudeIntegrator::GetEachAmpsHistograms(){
  return _integCalc->GetEachAmpsHistograms();
}

DalitzHistoSet FlexiFastAmplitudeIntegrator::interferenceHistoSet() const{
    return _integCalc->interferenceHistoSet();
}
void FlexiFastAmplitudeIntegrator::saveInterferenceHistograms(const std::string& prefix) const{
    return _integCalc->saveInterferenceHistograms(prefix);
}
std::vector<DalitzHistoSet> FlexiFastAmplitudeIntegrator::GetInterferenceHistograms(){
    return _integCalc->GetInterferenceHistograms();
}

void FlexiFastAmplitudeIntegrator::doFinalStats(Minimiser* mini){
  bool dbThis=true;
  if(dbThis) cout << "FlexiFastAmplitudeIntegrator::doFinalStats() called" << endl;
  _integCalc->doFinalStats(mini);
}
double FlexiFastAmplitudeIntegrator::getFractionChi2()const{
  return _integCalc->getFractionChi2();
}

/*
bool FlexiFastAmplitudeIntegrator::save(const std::string& fname)const{
  return _integCalc->save(fname);
}
*/

//
