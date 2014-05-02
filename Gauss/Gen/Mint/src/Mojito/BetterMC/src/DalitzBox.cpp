// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/DalitzBox.h"

#include "Mint/Minimiser.h"
#include "Mint/Minimisable.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/FitParameter.h"
#include "Mint/GeneralisedPareto.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;

bool bigDebug     = false;
bool fastTest     = false;
bool veryFastTest = false;
bool ultraFast    = true;

bool no_reuse = true;

class FindMaxFCN
  : public Minimisable
{
protected:

  FitParameter& _s0;
  FitParameter& _s1;
  FitParameter& _s2;
  FitParameter& _s3;
  FitParameter& _s4;
  
  DalitzBox* _box;

public:
  double getVal(){
    bool db=false;
    if(db) cout << " called getVal() " << endl;
    counted_ptr<DalitzEvent> evtPtr(_box->area().makeEventForOwner(_s0, _s1, _s2, _s3, _s4));
    if(db) cout << " got event Ptr " << evtPtr << endl;
    if(0 == evtPtr) return 0;
    if(_box->insideDaddysArea(*evtPtr)) return 0;

    if(db) cout << " evtPtr->print() ";
    if(db)evtPtr->print();
    if(db) cout << " that worked " << endl;
    double val = _box->ampsWithPhaseSpace((*evtPtr));
    if(db) cout << " got val " << val << endl;
    if(db) cout << " returning " << -val << endl;
    return -val;
  }

  FindMaxFCN(  FitParameter& s0
	       , FitParameter& s1
	       , FitParameter& s2
	       , FitParameter& s3
	       , FitParameter& s4
	       , DalitzBox* box
	       , MinuitParameterSet* mps
	       ) 
    : Minimisable(mps)
    , _s0(s0)
    , _s1(s1)
    , _s2(s2)
    , _s3(s3)
    , _s4(s4)
    , _box(box)
  {
  }
  
};

DalitzBox::DalitzBox() 
  :  _max_s0(-1), _max_s1(-1), _max_s2(-1), _max_s3(-1), _max_s4(-1)
  , _maxPhaseSpace(-9999)
  , _number(-1)
  , _name("noName")
  , _ready(false)
  , _area()
  , _pat()
  , _amps(0)
  , _rnd(0)
  , _height(-9999)
  , _eventList()
  , _nTries(0)
  , _nEventsForTries(0)
  , _daddy(0)
  , _guessedHeight(-9999)
  , _heightProblems(0)
  , _flatPhaseSpace(0)
{}


DalitzBox::DalitzBox(const DalitzEventPattern& pat
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  :  _max_s0(-1), _max_s1(-1), _max_s2(-1), _max_s3(-1), _max_s4(-1)
  , _maxPhaseSpace(-9999)
  , _number(-1)
  , _name("noName")
  , _ready(false)
  , _area(pat, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(-9999)
  , _eventList()
  , _nTries(0)
  , _nEventsForTries(0)
  , _daddy(0)
  , _guessedHeight(-9999)
  , _heightProblems(0)
  , _flatPhaseSpace(0)
{
  setAmps(amps);
}

DalitzBox::DalitzBox(const DalitzEventPattern& pat
		     , const DalitzCoordinate& limit
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  :  _max_s0(-1), _max_s1(-1), _max_s2(-1), _max_s3(-1), _max_s4(-1)
  , _maxPhaseSpace(-9999)
  , _number(-1)
  , _name("noName")
  , _ready(false)
  , _area(pat, limit, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(-9999)
  , _eventList()
  , _nTries(0)
  , _nEventsForTries(0)
  , _daddy(0)
  , _guessedHeight(-9999)
  , _heightProblems(0)
  , _flatPhaseSpace(0)
{
  setAmps(amps);
}

DalitzBox::DalitzBox(const DalitzEventPattern& pat
		     , const std::vector<DalitzCoordinate>& limits
		     , IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd)
  :  _max_s0(-1), _max_s1(-1), _max_s2(-1), _max_s3(-1), _max_s4(-1)
  , _maxPhaseSpace(-9999)
  , _number(-1)
  , _name("noName")
  , _ready(false)
  , _area(pat, limits, rnd)
  , _pat(pat)
  , _amps(0)
  , _rnd(rnd)
  , _height(-9999)
  , _eventList()
  , _nTries(0)
  , _nEventsForTries(0)
  , _daddy(0)
  , _guessedHeight(-9999)
  , _heightProblems(0)
  , _flatPhaseSpace(0)
{
  setAmps(amps);
}

DalitzBox::DalitzBox(const DalitzBox& other)
  :  _max_s0(other._max_s0)
  , _max_s1(other._max_s1)
  , _max_s2(other._max_s2)
  , _max_s3(other._max_s3)
  , _max_s4(other._max_s4)
  , _maxPhaseSpace(other._maxPhaseSpace)
  , _number(other._number)
  ,  _name(other._name)
  , _ready(other._ready)
  , _area(other._area)
  , _pat(other._pat)
  , _amps(other._amps)
  , _rnd(other._rnd)
  , _height(other._height)
  , _eventList(other._eventList) // expensive!
  , _nTries(other._nTries)
  , _nEventsForTries(other._nEventsForTries)
  , _daddy(other._daddy)
  , _guessedHeight(other._guessedHeight)
  , _heightProblems(other._heightProblems)
  , _flatPhaseSpace(other._flatPhaseSpace)
{
  //  _ready = false;
  //  cout << "copying box" << endl;
}

bool DalitzBox::setAmps(IReturnRealForEvent<IDalitzEvent>* amps){
  _ready = false;
  _amps = amps;
  _eventList.clear();
  if(0 == _amps) return false;
  return true;
}

DalitzBox::~DalitzBox(){
}

void DalitzBox::setDaddy(const DalitzBox* daddy){
  _ready = false;
  _daddy = daddy;
}

bool DalitzBox::insideArea(const DalitzEvent& evt) const{
  /*
  cout << "Hello from DalitzBox::insideArea()" << endl;
  cout << " my area is " << _area << endl;
  cout << ", or the same with (): " << area() << endl;
  cout << " and the event: " << evt << endl;
  cout << " Let's check if the event is inside the area: " << endl;
  */
  bool inside = area().isInside(evt);

  /*
  cout << "Goodbye from DalitzBox::insideArea(); returning " 
       << inside << endl;
  */

  return inside;
}

bool DalitzBox::insideDaddysArea(const DalitzEvent& evt) const{
  if(0 == _daddy) return false;
  return _daddy->insideMyOrDaddysArea(evt);
}

bool DalitzBox::insideMyOrDaddysArea(const DalitzEvent& evt) const{
  return insideArea(evt) || insideDaddysArea(evt);
}

int DalitzBox::makeFlatEventsKeepAll(int N){
  cout << "making " << N << " flat event" << endl;
  int counter = 0;
  int guessedWasteFactor = 110*5;
  if(fastTest) guessedWasteFactor = 150;

  for(int i=0; i< N*guessedWasteFactor; i++){
    _nTries ++;
    bool printdbg = (_nTries < 5 || 0 == _nTries%100000);
    if(printdbg) cout << " nTries " << _nTries << endl;
      
    counted_ptr<DalitzEvent> evt(area().makeEventForOwner());
    if(printdbg) cout << " made event" << endl;

    if(0 != evt && (! insideDaddysArea(*evt))){ // this should work
      if(printdbg)cout << " area OK " << endl;
      _eventList.Add(*evt);
      if(printdbg)cout << " added event" << endl;

      if(++counter >= N) break;
    }
  }

  cout << " got " << _eventList.size() << endl;
  return _nTries;
}

double DalitzBox::amps(IDalitzEvent& evt){
  bool dbThis=false;

  if(0 == _amps) return 1;
  if(dbThis) cout << " the event: " << endl;
  if(dbThis) evt.print();
  if(dbThis) cout << " ... now getting RealVal() " << endl;
  double val = _amps->RealVal(evt);
  if(dbThis) cout << "  all done, returning " << val << endl;
  return val;
}

double DalitzBox::phaseSpace(IDalitzEvent& evt_in){
  if(_flatPhaseSpace) return 1;
  return evt_in.phaseSpace();
}

double DalitzBox::ampsWithPhaseSpace(IDalitzEvent& evt_in){
  bool dbThis=false;

  double a = amps(evt_in);
  if(dbThis) cout << "amps " << a << endl;
  double ps = phaseSpace(evt_in);
  if(dbThis) cout << "ps " << ps << endl;

  return a*ps;
}

bool DalitzBox::estimateHeightMinuit(){
  MinuitParameterSet parset;

  /*
  // step 1: Find starting values:
  int minEvents=   10000000;
  int maxEvents=   10000001;
  int counter = 0;
  double max = -1;
  double s0=0.5, s1=0.5, s2=0.5, s3=0.5, s4=0.5;
  bool foundOne=false;
  cout << " starting loop" << endl;
  while((! (foundOne && counter > minEvents)) && counter < maxEvents){
    counter++;
    //    cout << " counter " << counter << endl;
    double tmp_s0 = _rnd->Rndm();
    double tmp_s1 = _rnd->Rndm();
    double tmp_s2 = _rnd->Rndm();
    double tmp_s3 = _rnd->Rndm();
    double tmp_s4 = _rnd->Rndm();
    counted_ptr<DalitzEvent> tmpEvtPtr(area().makeEventForOwner(tmp_s0, tmp_s1, tmp_s2, tmp_s3, tmp_s4));
    if(0 == tmpEvtPtr) continue;
    //cout << " found a non-zero event!!!" << endl;
    if(this->insideDaddysArea(*tmpEvtPtr)) return 0;

    foundOne=true;
    _amps->setEvent(&(*tmpEvtPtr));
    double val = _amps->RealVal();
    _amps->resetEventRecord();
    if(val > max){
      max=val;
      s0 = tmp_s0; s1=tmp_s1, s2=tmp_s2, s3=tmp_s3, s4=tmp_s4;
    }
  }

  if(max < 0){
    cout << " didn't find starting point "
	 << " giving up; "
	 << endl;
    _height = -1;
    return true;
  }

  cout << " max = " << max << " at "
       << s0 << ", " << s1 << ", " << s2 << ", " << s3 << ", " << s4 << endl;

  */
  FitParameter fit_scale0("scale0"
			  , 0
			  , _max_s0
			  , 0.2
			  , -0.05
			  , 1.05
			  , &parset
			  );
  FitParameter fit_scale1("scale1"
			  , 0
			  , _max_s1
			  , 0.2
			  , -0.05
			  , 1.05
			  , &parset
			  );
  FitParameter fit_scale2("scale2"
			  , 0
			  , _max_s2
			  , 0.2
			  , -0.05
			  , 1.05
			  , &parset
			  );
  FitParameter fit_scale3("scale3"
			  , 0
			  , _max_s3
			  , 0.2
			  , -0.05
			  , 1.05
			  , &parset
			  );
  FitParameter fit_scale4("scale4"
			  , 0
			  , _max_s4
			  , 0.2
			  , -0.05
			  , 1.05
			  , &parset
			  );

  FindMaxFCN fcn(fit_scale0
		 , fit_scale1
		 , fit_scale2
		 , fit_scale3
		 , fit_scale4
		 , this, &parset);
  
  Minimiser mini(&fcn);
  //  mini.Command("Set Strategy 2");
  mini.prepFit();
  mini.Command("Simplex 10000000");
  mini.Command("MIGRAD");

  double safetyMargin = 0;

  _height = -(mini.getFCNVal() * (1.0 + safetyMargin));

  if(_height <= 0.0) _height = -1.0;
  cout << " setting height to " << _height << endl;
  return true;
}

bool DalitzBox::estimateHeight(std::vector<double>& vals
			       ){
  bool dbThis=false;
  int maxEvents =   100000;
  int maxTries  = 10000000;
  double meanPhaseSpaceSum = 0;
  if(fastTest){
    maxEvents =   10000;
    maxTries  = 1000000;
  }
  if(veryFastTest || ultraFast){
    maxEvents =    1000;
    maxTries  = 1000000;
  }

  if(guessedHeight() > 0){
    _height = guessedHeight();
    return 0;
  }

  //  double saveFactor =  2.0;

  cout << "DalitzBox::estimateHeight" << endl;
  //time_t startTime = time(0);
  
  double max = -9999;

  int numEvents=0, counter=0;
  while(numEvents < maxEvents && counter < maxTries){
    counter++;
    double tmp_s0 = _rnd->Rndm();
    double tmp_s1 = _rnd->Rndm();
    double tmp_s2 = _rnd->Rndm();
    double tmp_s3 = _rnd->Rndm();
    double tmp_s4 = _rnd->Rndm();
    counted_ptr<DalitzEvent> tmpEvtPtr(area().makeEventForOwner(tmp_s0, tmp_s1, tmp_s2, tmp_s3, tmp_s4));
    if(0 == tmpEvtPtr) continue;
    if(dbThis) cout << "estimate height got event "
		    << " printing it: " << endl;
    if(dbThis) cout << *tmpEvtPtr << endl;

    //cout << " found a non-zero event!!!" << endl;
    if(this->insideDaddysArea(*tmpEvtPtr)) continue;

    numEvents++;


    DalitzEvent tmpEvt(*tmpEvtPtr);
    double val = ampsWithPhaseSpace(tmpEvt);
    vals.push_back(val);
    if(val > max){
      max=val;
      _max_s0 = tmp_s0; 
      _max_s1 = tmp_s1; 
      _max_s2 = tmp_s2;
      _max_s3 = tmp_s3;
      _max_s4 = tmp_s4;
    }
    double p =  phaseSpace(tmpEvt);

    meanPhaseSpaceSum += p;
    if(p > _maxPhaseSpace){
      _maxPhaseSpace = p;
    }
  };

  double meanPhaseSpace = meanPhaseSpaceSum/((double)numEvents);
  cout << "mean phase space " << meanPhaseSpace
       << ", max: " << _maxPhaseSpace << endl;

  //double epsilon = 0.25;
  
  double CL = 1.0 - 1.e-7;
  std::cout << "actual max = " << max << std::endl;

  if(numEvents < 50){
    cout << " no Paret of only " << _eventList.size()
	 << " events. Taking 5*max = 5*" << max 
	 << " = " << 5*max
	 << " and return false." << endl;
    _height = 5.0 * max;
    return false;
  }

  double actualMax=-1, paretoMax=-1;

  max = generalisedPareto_estimateMaximum(vals
					  , CL
					  , actualMax
					  , paretoMax
					  );
  
  std::cout << " Pareto max " << paretoMax << std::endl;
  /*
  max *= 1.0 + epsilon;
  std::cout << "Now added " << epsilon * 100 << "% for safety "
	    << max << std::endl;

  max *= saveFactor;

  std::cout << "and multiplied by safety factor " 
	    << saveFactor
	    << " giving " << max 
	    << endl;
  */
  cout << "compare to guessed height: " 
       << guessedHeight()*_maxPhaseSpace << endl;
  _height = max;

  if(fabs( (paretoMax - actualMax)/actualMax) > 0.3 ){
    return false;
  }

  return true;
}

bool DalitzBox::estimateHeight_old(std::vector<double>& vals
			       ){
  double saveFactor = 2.0;

  cout << "DalitzBox::estimateHeight" << endl;
  if(false && bigDebug){
    vals.resize(_eventList.size(), 1);
    _height=1;
    return true;
  }
  if(_eventList.empty()){
    _height = 0;
    return false;
  }
  time_t startTime = time(0);
  
  unsigned int counter=0;

  vals.clear();

  vals.resize(_eventList.size());

  double max = -9999;

  double maxPhaseSpace = -9999;

  for(unsigned int i=0; i < _eventList.size(); i++){
    IDalitzEvent& evt = _eventList[i];
     double p = evt.phaseSpace();
    if(p > maxPhaseSpace || 0 == counter){
      maxPhaseSpace = p;
    }
    double d=ampsWithPhaseSpace(evt);

    if(d > max || 0 == counter){
      max=d;
      //      _maxEvent = *(_eventList.getEvent());
    }
    vals[i] = d;
    if(i < 5 || 0 == i%(_eventList.size()/20 + 1)){
      std::cout << " calculated amps for event " 
		<< i 
		<< ", its value is " << d
		<< std::endl;
      double deltaT = difftime(time(0), startTime);
      std::cout << " this took " << deltaT << " s";
      if(deltaT > 0.5){
	std::cout << "; rate (before throwing away) = " 
		  << i/deltaT
		  << " evts/s";
      }
      std::cout << std::endl;
    }
  };

  cout << " max phase space: " << maxPhaseSpace << endl;

  double epsilon = 0.25;
  
  double CL = 1.0 - 1./_eventList.size();
  std::cout << "actual max = " << max << std::endl;

  if(_eventList.size() < 50){
    cout << " no Paret of only " << _eventList.size()
	 << " events. Taking 5*max = 5*" << max 
	 << " = " << 5*max
	 << " and return false." << endl;
    _height = 5.0 * max;
    return false;
  }

  double actualMax=-1, paretoMax=-1;

  max = generalisedPareto_estimateMaximum(vals
					  , CL
					  , actualMax
					  , paretoMax
					  );
  
  std::cout << " Pareto max " << paretoMax << std::endl;
  max *= 1.0 + epsilon;
  std::cout << "Now added " << epsilon * 100 << "% for safety "
	    << max << std::endl;

  max *= saveFactor;

  std::cout << "and multiplied by safety factor " 
	    << saveFactor
	    << " giving " << max 
	    << endl;
  cout << "compare to guessed height: " 
       << guessedHeight()*maxPhaseSpace << endl;
  _height = max;

  if(fabs( (paretoMax - actualMax)/actualMax) > 0.3 ){
    return false;
  }

  return true;
}

bool DalitzBox::makeStarterSet(){
  double safetyFactor = 2.5;

  time_t startTime = time(0);
  cout << "making starter set" << endl;
  // unsigned int maxEvents = 400000;
  int NperLoop           =  20000;
  //  int maxTries           = maxEvents * 110;
  int maxLoop            =  5;
  if(fastTest){
    NperLoop = 10000;
    maxLoop  = 3;
  }
  if(veryFastTest){
    NperLoop = 1000;
    maxLoop  = 1;
  }

  bool successfulHeightEstimate = false;
  std::vector<double> vals;

  int counter = 0;
  double prevHeight = -0.1;
  double heightErrorEstimate = 0.1;

  bool doOldHeight=(guessedHeight() < 10);
  bool doMinuitHeight=false;
  if(doOldHeight){
    do{    
      //  makeFlatEventsKeepAll(NperLoop);
      successfulHeightEstimate = estimateHeight(vals);
      heightErrorEstimate 
	= fabs(prevHeight - _height)/( 0.5*(fabs(prevHeight) + fabs(_height) ));

      /*
      successfulHeightEstimate |= heightErrorEstimate < 0.25;
      // the above line means: I believe it if pareto and actual
      // are close - or if at least pareto is consistent.
      */

      successfulHeightEstimate &= heightErrorEstimate < 0.25;
      // the above line means: I believe it if pareto and actual
      // are close AND if Pareto is consistent.
      counter++;
      double deltaT = difftime(time(0), startTime);
      cout << " making " << counter 
	   << " " << NperLoop << "-events starter set(s)"
	   << " took " << deltaT/60 << " min" << endl;
      cout << " current heightErrorEstimate: " << heightErrorEstimate << endl;
      prevHeight = _height;
      // no event inside box - box must have 0 volume
      // (most likely overlap 100% with other volume)
    }while(counter < maxLoop
	   && (! successfulHeightEstimate)
	   );
    
    cout << " DONE \"old\" height guess, now MINUIT " << endl;
  }
  double search_height = _height;
  double minuit_height = -1;
  if(doMinuitHeight){
    successfulHeightEstimate |= estimateHeightMinuit();
    minuit_height = _height;
  }


  _height = ( search_height > minuit_height ? search_height : minuit_height);
  _height = ( guessedHeight() > _height     ? guessedHeight() : _height);

  cout << "Different Heights:"
       << "\n\t  search_height " << search_height
       << "\n\t  minuit_height " << minuit_height
       << "\n\t guessed height " << guessedHeight()
       << "\n\t  chosen height " << _height
       << endl;

  if(! successfulHeightEstimate){
    cout << "DalitzBox::makeStarterSet "
	 << "WARNING - no reliable estimate of box-height"
	 << endl;
    cout << " I'll multiply it by " << 1.0 + 2*heightErrorEstimate
	 << ", and continue..." << endl;
    //    _height *= 1.0 + 2.0*heightErrorEstimate;
  }else{
    cout << "DalitzBox::makeStarterSet "
	 << "I think I found a reliable Height Estimate of "
	 << _height
	 << endl;
    /*    _height *= 1.0 + 2.0*heightErrorEstimate;
	  cout << "For safety, I'll add 2* the Error estimate of "
	  << heightErrorEstimate << " which gives a final height of "
	  << _height
	  << endl;
    */
  }
  cout << " appliying safety factor of " << safetyFactor << endl;
  _height *= safetyFactor;
  cout << "final height " << _height << endl;

  //throwAwayData(vals);

  return true;
}

int DalitzBox::throwAwayData(const std::vector<double>& vals){
  if(false && bigDebug) return 0;
  time_t startTime = time(0);
  
  if(vals.size() != _eventList.size()){
    cout << " ERROR in  DalitzBox::throwAwayData: "
	 << " vals.size()=" << vals.size()
	 << ", but _eventList.size()=" 
	 << _eventList.size()
	 << endl;
    throw "this is bad.";
  }
  
  DalitzEventList newList;

  int rememberSize = _eventList.size();

  unsigned int counter=0;

  for(unsigned int i=0; i < _eventList.size(); i++){
    IDalitzEvent& evt(_eventList[i]);

    double d=ampsWithPhaseSpace(evt);
    //    double d = vals[counter]; 

    if(_rnd->Rndm()*_height < d){
      newList.Add( _eventList[i] );
    }
    if(counter < 10 || 0 == counter%(_eventList.size()/20 + 1)){
      std::cout << " remembering amps for event " 
		<< counter 
		<< ", its value is " << d
		<< std::endl;
    }
    counter++;
  }

  _eventList = newList;
  std::cout << "now my size has changed to " << _eventList.size()
	    << std::endl;
  std::cout << " So the waste factor for \n" << this->name() << " is ";
  if(_eventList.size() > 0) std::cout << rememberSize/_eventList.size();
  else std::cout << " infinity! - that's big!";
  std::cout << std::endl;
  
  
  double deltaTFinal = difftime(time(0), startTime);
  std::cout << " this took " << deltaTFinal/60.0 << " min";
  if(deltaTFinal > 0){
    std::cout << " rate = " << (_eventList.size()/deltaTFinal) << " evts/s"
	      << " or " << (_eventList.size()/deltaTFinal) *60 << " evts/m"
	      << " or " << (_eventList.size()/deltaTFinal) *60.*60./1000. 
	      << "k evts/h";
  }
  std::cout << std::endl;
  
  std::cout << " ---------------\n " << std::endl;

  return _eventList.size();
}

double DalitzBox::volume() const{
  double a = area().size();
  cout << " volume: returning area " << a
       << " times height " << _height
       << " = " << a*_height
       << endl;
  return a * _height;
}
void DalitzBox::getReady(){
  time_t startTime = time(0);

  cout << " DalitzBox::getReady() " << *this << endl;
  
  _nTries = 0;
  _nEventsForTries=0;
  _eventList.clear();

  makeStarterSet();
  _nEventsForTries = _eventList.size();
  
  double deltaT = difftime(time(0), startTime);
  cout << " DalitzBox::getRead(): am ready. This took " 
       << deltaT/60 << " min" << endl;

  if(no_reuse) _eventList.clear();

  _ready = true;
}

counted_ptr<DalitzEvent> DalitzBox::tryEventForOwner(){
  if(! _ready) getReady();
  
  counted_ptr<DalitzEvent> evt(0);

  if(_eventList.empty()){
    evt = tryNewEvent();
  }else{
    evt = tryEventFromList();
  }
  return evt;
}

double DalitzBox::getEventsPdf(DalitzEvent& evt){

  if(0 == _amps || bigDebug) return evt.phaseSpace();

  double val = _amps->RealVal(evt) * evt.phaseSpace();

  return val;
}

counted_ptr<DalitzEvent> DalitzBox::tryNewEvent(){

  counted_ptr<DalitzEvent> evt( area().makeEventForOwner() );

  if( 0 == evt){                         // bad ptr
    return counted_ptr<DalitzEvent>(0);
  }
  if(insideDaddysArea(*evt)){            // out of area
    return counted_ptr<DalitzEvent>(0);
  }

  double val = getEventsPdf(*evt);
  if(val > _height){
    _heightProblems++;
    cout << "ERROR in DalitzBox::tryNewEvent()\n" << (*this)
	 << "\n   val > _height: " << val << " > " << _height
	 << "\n  (for phase space = " << evt->phaseSpace() << ")."
	 << "\n  if _height had been correctly estimated,"
	 << " this should not be possible." << endl;
    cout << " This has happened " << _heightProblems 
	 << " times in this box " << endl;
  }
  if(_rnd->Rndm()*_height > val){ // then we don't take it.
    evt = counted_ptr<DalitzEvent>(0);
  }
  return evt;
}

counted_ptr<DalitzEvent> DalitzBox::tryWeightedEventForOwner(){
  bool dbThis=false;
  counted_ptr<DalitzEvent> evt( area().makeEventForOwner() );

  if( 0 == evt){                         // bad ptr
    return counted_ptr<DalitzEvent>(0);
  }
  if(insideDaddysArea(*evt)){            // out of area
    return counted_ptr<DalitzEvent>(0);
  }

  if(dbThis)cout << " DalitzBox::tryWeightedEventForOwner() "
		 << " got event with ptr "
		 << evt
		 << ", setting val: " << endl;
  if(dbThis)cout << " now printing event: "
		 << *evt
		 << endl;
  double val = ampsWithPhaseSpace(*evt);
  if(dbThis)cout << " val = " << val << ", now setting weight " << endl;
  evt->setWeight(val/_height);
  if(dbThis)cout << " done weight, now generator..." << endl;
  evt->setGeneratorPdfRelativeToPhaseSpace(amps(*evt));
  if(dbThis)cout << " all OK, returning " << evt << endl;
  return evt;
}

counted_ptr<DalitzEvent> DalitzBox::tryEventFromList(){
  // the list has already been subjected to
  // data throwing away.
  // _nTries were made, and _nEventsForTries are
  // left over. So each event in the list represents
  // on average _nTries/_nEventsForTries tries;

  double rndNumber = _rnd->Rndm()*_nTries;
  if(rndNumber > (double) _nEventsForTries){
    return counted_ptr<DalitzEvent>(0);
  }
  return popEventFromList();  
}

counted_ptr<DalitzEvent> DalitzBox::popEventFromList(){
  if(_eventList.empty()){
    return counted_ptr<DalitzEvent>(0);
  }
  counted_ptr<DalitzEvent> evt(new DalitzEvent(_eventList[_eventList.size()-1]));
  _eventList.resize(_eventList.size()-1);
  return evt;
}

std::vector<DalitzBox> DalitzBox::split(unsigned int nWays
					) const{
  std::vector<MappedDalitzArea> areaList(_area.split(nWays));

  std::vector<DalitzBox> newSet;
  if(areaList.empty()) return newSet;
  bool foundCtr=false;
  for(unsigned int i=0; i < areaList.size(); i++){
    DalitzBox newBox(*this);
    newBox.area() = areaList[i];
    if( ! (newBox.area().isInside((this->area().centre())))){
      //      newBox.setGuessedHeight(0.6*guessedHeight());
      newBox.setGuessedHeight(-1);
    }else{
      foundCtr=true;
    }
    newSet.push_back(newBox);
  }
  if(false && (! foundCtr)){
    cout << "ERROR split box " << *this << " " << nWays << " ways,"
	 << " but center: is gone." << endl;
    cout << "newSet\n: " << newSet << endl;
  }
  return newSet;
}

std::vector<DalitzBox> DalitzBox::splitIfWiderThan(double maxWidth
						   ) const{
  std::vector<MappedDalitzArea> areaList(_area.splitIfWiderThan(maxWidth));

  std::vector<DalitzBox> newSet;
  if(areaList.empty()) return newSet;
  for(unsigned int i=0; i < areaList.size(); i++){
    DalitzBox newBox(*this);
    newBox.area() = areaList[i];
    if( ! (newBox.area().isInside((this->area().centre())))){
      //      newBox.setGuessedHeight(0.6*guessedHeight());
      newBox.setGuessedHeight(-1);
    }
    newSet.push_back(newBox);
  }
  return newSet;
}

void DalitzBox::print(std::ostream& os) const{
  os << "DalitzBox: " << name() << " number " << number()
     << "\n area " << area()
     << "\n guessed height " << guessedHeight()
     << "\n height " << height()
     << "\n daddy: ";
  if(0 == _daddy){
    os << " none";
  }else{
    os << " number " << _daddy->number();
  }
}

bool DalitzBox::setRnd(TRandom* rnd){
  _rnd = rnd;
  area().setRnd(rnd);
  return true;
}

ostream& operator<<(ostream& os, const DalitzBox& box){
  box.print(os);
  return os;
}
//
