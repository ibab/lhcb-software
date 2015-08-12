// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include "Mint/DalitzMCIntegrator.h"

#include "TRandom.h"
#include "Mint/IWeightedEvent.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;


/*
  Warning: This piece of code is much slower and less accurate than
  "FastAmplitudeIntegrator", as used by DalitzPdfBaseFastInteg.
  Still, the code here is good for x-checks.
 */
DalitzMCIntegrator::DalitzMCIntegrator( const DalitzEventPattern& pattern
					, IReturnRealForEvent<IDalitzEvent>* weightFunction
					, IEventGenerator<IDalitzEvent> * generator
					, TRandom* rnd
					, double precision
					)
  : _Ncalls(0)
  , _sumT(0)
  , _mean(0)
  , _variance(0)
  , _numEvents(0)
  , _weightSum(0)
  , _initialised(0)
  , _iw(weightFunction)
{
  initialise(pattern, weightFunction, generator, rnd, precision);
}
DalitzMCIntegrator::DalitzMCIntegrator()
  : _Ncalls(0)
  , _sumT(0)
  , _mean(0)
  , _variance(0)
  , _numEvents(0)
  , _weightSum(0)
  , _initialised(0)
  , _iw(0)
{
}

bool DalitzMCIntegrator::initialise(const DalitzEventPattern& pattern
				    , IReturnRealForEvent<IDalitzEvent>* weightFunction
				    , IEventGenerator<IDalitzEvent> * generator
				    , TRandom* rnd
				    , double precision
				    ){
  _Ncalls = 0;
  _sumT   = 0.0;
  _pat    = pattern;
  _w      = weightFunction;
  _rnd    = rnd;
  _precision = precision;
  
  _generator = generator;

  _iw.setWeight(weightFunction);

  _initialised = true;


  generateEnoughEvents();// must come last.

  return _initialised;
}
bool DalitzMCIntegrator::resetIntegrand(IReturnRealForEvent<IDalitzEvent>* 
					weightFunction
					){
  _w   = weightFunction;
  _iw.setWeight(weightFunction);
  return true;
}

int DalitzMCIntegrator::updateEventSet(int Nevents){
  if(! _initialised){
    cout << " DalitzMCIntegrator::updateEventSet "
	 << " need to know pattern first." << endl;
    return 0;

  }
  int missingEvents = Nevents - _events.size();
  cout << "missing events: " << missingEvents 
       << " = " << Nevents 
       << " - " << _events.size()
       << endl;
  if(missingEvents > 0){
    addEvents(missingEvents);
  }
  return _events.size();
}

int DalitzMCIntegrator::addEvents(int Nevents){
  bool dbThis=false;
  if(Nevents <= 0) return _events.size();

  if(0 == _generator){
    _events.generatePhaseSpaceEvents(Nevents, _pat, _rnd);
    return _events.size();
  }
  
  int N_success = 0;
  for(int i=0; i < Nevents*1000 && N_success < Nevents; i++){
    //counted_ptr<IDalitzEvent> ptr(_generator->newEvent());
    counted_ptr<DalitzEvent> ptr(new DalitzEvent(_generator->newEvent().get()));
    if(dbThis) cout << "got event with ptr: " << ptr << endl;
    if(0 != ptr){
      _events.push_back(ptr); // change event list to list of counted ptrs
      N_success++;
      int printEvery = 1000;
      if(N_success > 10000) printEvery=10000;
      bool printout = (N_success%printEvery == 0);
      if(printout){
	cout << " DalitzMCIntegrator::addEvents: added " << N_success 
	     << " th event" << endl;
      }
    }
  }
  cout << " added " << Nevents << " _eventList.size() "
       << _events.size() << endl;
  return _events.size();
}

double DalitzMCIntegrator::evaluateSum(){
  /*
  if(! _initialised){
    cout << " DalitzMCIntegrator::evaluateSum "
	 << " need to know pattern first." << endl;
    return 0;
  }

  if(_events.empty()){
    cout << "WARNING in DalitzMCIntegrator::evaluateSum()"
	 << " no events!" << endl;
    _mean = _variance = 0;
    return 0;
  }
  */

  double sum   = 0;
  double sumsq = 0;
  double ws  =0;
  _weightSum = 0;
  //int printEveryNEvents = (_events.size()/4);
  //if(printEveryNEvents < 1) printEveryNEvents = 1;

  //time_t tstart = time(0);

  //#pragma omp parallel for reduction(+:sum, sumsq, ws);
  for(unsigned int N=0; N < _events.size(); N++){
    //DalitzEvent thisEvt(_events[N]);
    /*  
    double ps = thisEvt.phaseSpace();
    if(ps <= 0.0){
      cout << "WARNING in DalitzMCIntegrator::evaluateSum()"
	   << " event with phase space = " << ps << endl;
      continue; // should not happen.
    }
    */
    double weight =  _events[N].getWeight() / _events[N].getGeneratorPdfRelativeToPhaseSpace();
    double val = _iw.RealVal(_events[N]); // _w->RealVal() * weight;

    sum   += val;
    sumsq += val*val;

    ws += weight;// /ps;

    /*  
    if(N%printEveryNEvents == 0){
      cout << "DalitzMCIntegrator::evaluateSum() N = " << N 
	   << " val = " << val 
	   << " _w->RealVal(thisEvt)*weight " << _w->RealVal(thisEvt)*weight 
	   << endl;
    }
    */
  }

  _weightSum = ws;

  double fN       = (double) _events.size();
  _mean           = sum   / fN;
  double meanOfSq = sumsq / fN;
  _variance       = (meanOfSq - _mean * _mean)/fN;

  if(_weightSum > 0){
    double sf = fN/_weightSum;
    _mean *= sf;
    _variance *= sf*sf;
  }

  /*
  double delT = difftime(time(0), tstart);
  double sigma = -9999;
  if(_variance >= 0) sigma = sqrt(_variance);
  double actualPrecision = -9999;
  if(_mean != 0 && sigma > 0) actualPrecision = sigma/_mean;
  cout << " DalitzMCIntegrator::evaluateSum() for " << N << " events:"
       << "\n     > getting: " << _mean << " +/- " << sigma
       << "\n     > precision: requested: " << _precision*100
       << "%, actual: " << actualPrecision*100 << "%"
       << "\n     > This took " << delT << " s."
       << endl;
  */
  return _mean;
}

int DalitzMCIntegrator::determineNumEvents(){
  updateEventSet(_minEvents);
  evaluateSum();

  double safetyFactor = 1.15;
  double maxVariance = _precision*_precision * _mean*_mean;
  _numEvents = _events.size() * ( ((int)(_variance*safetyFactor/maxVariance)) + 1);
  cout << " DalitzMCIntegrator::determineNumEvents():"
       << "\n    > mean: " << _mean << ", rms " << sqrt(_variance)
       << "\n    > currently, our precision is " << sqrt(_variance)/_mean
       << "\n    > to achieve requested pecision of " << _precision << ","
       << "\n    > I think we'll need " << (_variance*safetyFactor/maxVariance)
       << " times that, i.e. " << _numEvents << " events" << endl;

  return _numEvents;
}

int DalitzMCIntegrator::generateEnoughEvents(){
  addEvents(300000);
  //updateEventSet(_minEvents);
  //determineNumEvents();
  //updateEventSet(_numEvents);
  return _events.size();
}

double DalitzMCIntegrator::getVal(){
  _Ncalls++;

  int                  printEvery =     10;
  if(_Ncalls >    100) printEvery =    100;
  if(_Ncalls >   1000) printEvery =   1000;
  if(_Ncalls >  10000) printEvery =  10000;
  if(_Ncalls > 100000) printEvery = 100000;
  bool printout = _Ncalls < 10 || (0 == _Ncalls%printEvery);

  time_t tstart = time(0);
  evaluateSum();
  double delT = difftime(time(0), tstart);

  _sumT += delT;

  if(_Ncalls > 0 && printout){
    cout << "DalitzMCIntegrator::getVal, " << _Ncalls << "th call: Returning " << _mean
	 << " this call took " << delT << " s,"
	 << " average call " << _sumT/_Ncalls << " s, "
	 << " for " << _events.size() << " events."
	 << endl;
  }

  return _mean;
}

void DalitzMCIntegrator::doFinalStats(MINT::Minimiser*){};

DalitzMCIntegrator::
integrationWeight::integrationWeight(IReturnRealForEvent<IDalitzEvent>* externalPdf) 
  : _externalPdf(externalPdf)
{
  cout << "after construction, externalPdf = " << _externalPdf << endl;
}

double DalitzMCIntegrator::
integrationWeight::RealVal(IDalitzEvent& evt){
  double den    = evt.getGeneratorPdfRelativeToPhaseSpace();
  double weight = evt.getWeight();
  if(0 != den) weight /= den;
  double val = _externalPdf->RealVal(evt) * weight;

  return val;
}

void  DalitzMCIntegrator::
integrationWeight::setWeight(IReturnRealForEvent<IDalitzEvent>* pdf){
  _externalPdf = pdf;
}

//
