// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzBWBoxSet.h"

#include "Mint/phaseSpaceIntegrals.h"
#include "Mint/ReturnWeight.h"
#include "Mint/DiskResidentEventList.h"

#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;
using namespace MINT;

double DalitzBWBoxSet::__phaseSpaceFracDefaultValue=0.25;
// phaseSpaceFrac is the fraction of events generated according to
// phase space (in addition to any flat phase-space component the
// model might have). This phase-space component makes sure we don't
// miss out any part of phase space where the (approximate) amplitude
// model might have some artificial "holes" etc.

DalitzBWBoxSet::DalitzBWBoxSet(IReturnRealForEvent<IDalitzEvent>* amps, TRandom* r)
  : std::vector<DalitzBWBox>()
  , _maxWeightEstimate(-9999.0)
  , _maxWeightInSample(-9999.0)
  , _ampSum(amps)
  , _ready(false)
  , _volumeProbs()
  , _rnd(r)
  , _phaseSpaceFrac(DalitzBWBoxSet::__phaseSpaceFracDefaultValue)
  , _phaseSpaceIntegral(-9999)
  , _psbox()
  , _pick_ps_prob(-9999)
{}
DalitzBWBoxSet::DalitzBWBoxSet(TRandom* r)
  : std::vector<DalitzBWBox>()
  , _maxWeightEstimate(-9999.0)
  , _maxWeightInSample(-9999.0)
  , _ampSum(0)
  , _ready(false)
  , _volumeProbs()
  , _rnd(r)
  , _phaseSpaceFrac(DalitzBWBoxSet::__phaseSpaceFracDefaultValue)
  , _phaseSpaceIntegral(-9999)
  , _psbox()
  , _pick_ps_prob(-9999)
{}

DalitzBWBoxSet::DalitzBWBoxSet(const DalitzBWBoxSet& other)
  : IEventGenerator<IDalitzEvent>()
  , IUnweightedEventGenerator<IDalitzEvent>()
  , std::vector<DalitzBWBox>(other)
  , _eventPtrList(other._eventPtrList)
  , _maxWeightEstimate(other._maxWeightEstimate)
  , _maxWeightInSample(other._maxWeightInSample)
  , _ampSum(other._ampSum)
  , _ready(other._ready)
  , _volumeProbs(other._volumeProbs)
  , _rnd(other._rnd)
  , _phaseSpaceFrac(other._phaseSpaceFrac)
  , _phaseSpaceIntegral(other._phaseSpaceIntegral)
  , _psbox(other._psbox)
  , _pick_ps_prob(other._pick_ps_prob)
{}

double DalitzBWBoxSet::fullPdf(DalitzEvent& evt){ // (but w/o phase space factor)
  bool dbThis=false;
  if(dbThis) cout << "DalitzBWBoxSet::fullPdf called" << endl;
  if(0 == _ampSum) return 0;
  double returnVal = _ampSum->RealVal(evt);
  if(dbThis) cout << "DalitzBWBoxSet::fullPdf result: " 
		  << returnVal << endl;
  return returnVal;
}

void DalitzBWBoxSet::add(DalitzBWBox& box){
  _ready = false;
  box.setRnd(_rnd);
  this->push_back(box);
}

void DalitzBWBoxSet::add(DalitzBWBoxSet& boxes){
  _ready = false;
  for(unsigned int i=0; i<boxes.size(); i++){
    this->add(boxes[i]);
  }
}
bool DalitzBWBoxSet::setRnd(TRandom* rnd){
  if(0 == rnd) _rnd = gRandom;
  else _rnd = rnd; // this *should* do it, but for safety:
  for(unsigned int i=0; i< this->size(); i++){
    (*this)[i].setRnd(_rnd);
  }
  return true;
}

bool DalitzBWBoxSet::ensureFreshEvents(){
  setRnd(_rnd); // safety paranoia - all boxes should be using this already
  _rnd->SetSeed(time(0)*2);
  return true;
}
bool DalitzBWBoxSet::checkIntegration()const{
  bool s=true;

  double prevV=0;
  for(unsigned int i=0; i< this->size(); i++){
    if(prevV == _volumeProbs[i]) continue;
    prevV=_volumeProbs[i];
    cout << "checking integration for box "
	 << (*this)[i].name() << endl;
    s &= (*this)[i].checkIntegration();
    cout << "============================="
	 << endl;
  }
  return s;
}

bool DalitzBWBoxSet::am_I_generating_what_I_think_I_am_generating(int Nevents){
  cout << "Hello from DalitzBWBoxSet::am_I_generating_what_I_think_I_am_generating"
       << " with " << Nevents << " events to generate for each case" << endl;

  DiskResidentEventList generated_approxPDF_events("generated_approxPDF_events.root"
						   , "RECREATE");
  DiskResidentEventList generatedFlat_weighted_approxPDF_events("generatedFlat_weighted_approxPDF_events.root"
								, "RECREATE");

  int printEvery = min(Nevents/10, 10000);
 
  for(int i=0; i < Nevents; i++){
    generated_approxPDF_events.Add(*makeWeightedApproxEventForOwner());
    if(0 == i%printEvery) cout << "done event " << i << endl;
  }
  cout << "generated approxPDF_events"<< endl;

  generated_approxPDF_events.save();
  DalitzHistoSet datGen= generated_approxPDF_events.weightedHistoSet();
  datGen.save("generated_approxPDF_histos.root");
  datGen.draw("generated_approxPDF_histos");

  int rwfactor=1;
  for(int i=0; i < Nevents*rwfactor; i++){
    counted_ptr<DalitzEvent> evtPtr(phaseSpaceEvent());
    evtPtr->setWeight(evtPtr->getWeight()*genValueNoPs(*evtPtr));
    generatedFlat_weighted_approxPDF_events.Add(*evtPtr);
    if(0 == i%(printEvery*rwfactor)) cout << "done event " << i << endl;
  }
  cout << "generated generatedFlat_weighted_approxPDF_events" << endl;
  generatedFlat_weighted_approxPDF_events.save();;
  DalitzHistoSet datWeight = generatedFlat_weighted_approxPDF_events.weightedHistoSet();
  datWeight.save("generatedFlat_weighted_approxPDF_histos.root");
  datWeight.draw("generatedFlat_weighted_approxPDF_histos");
  datGen.drawWithFitNorm(datWeight, "genDotsWeightLine_", "eps", "E1 SAME");
  
  cout << "am_I_generating_what_I_think_I_am_generating: all done" << endl;

  return true;
}
bool DalitzBWBoxSet::compareGenerationMethodsForFullPDF(int Nevents){
  cout << "Hello from DalitzBWBoxSet::compareGenerationMethodsForFullPDF"
       << " with " << Nevents << " events to generate for each case" << endl;

  DiskResidentEventList generated_fullPDF_efficient_weighted_events("generated_fullPDF_efficient_weighted_events.root"
						   , "RECREATE");
  DiskResidentEventList generatedFlat_fullPDF_flat_weighted_events("generated_fullPDF_flat_weighted_events.root"
								, "RECREATE");

  int printEvery = min(Nevents/10, 10000);
 
  for(int i=0; i < Nevents; i++){
    generated_fullPDF_efficient_weighted_events.Add(*makeWeightedEventForOwner());
    if(0 == i%printEvery) cout << "done event " << i << endl;
  }
  cout << "generated approxPDF_events"<< endl;

  generated_fullPDF_efficient_weighted_events.save();
  DalitzHistoSet datGen= generated_fullPDF_efficient_weighted_events.weightedHistoSet();
  datGen.save("generated_fullPDF_efficient_weighted_histos.root");
  datGen.draw("generated_fullPDF_efficient_weighted");

  int rwfactor=1;
  for(int i=0; i < Nevents*rwfactor; i++){
    counted_ptr<DalitzEvent> evtPtr(phaseSpaceEvent());
    evtPtr->setWeight(evtPtr->getWeight()*fullPdf(*evtPtr));
    generatedFlat_fullPDF_flat_weighted_events.Add(*evtPtr);
    if(0 == i%(printEvery*rwfactor)) cout << "done event " << i << endl;
  }
  cout << "generated generatedFlat_fullPDF_flat_weighted_events" << endl;
  generatedFlat_fullPDF_flat_weighted_events.save();;
  DalitzHistoSet datWeight = generatedFlat_fullPDF_flat_weighted_events.weightedHistoSet();
  datWeight.save("generated_fullPDF_efficient_weighted_histos.root");
  datWeight.draw("generated_fullPDF_efficient_weighted_events");
  datGen.drawWithFitNorm(datWeight, "efficientBlue_FlatRed_", "eps", "E1 SAME");
  
  cout << "am_I_generating_what_I_think_I_am_generating: all done" << endl;

  return true;
}

void DalitzBWBoxSet::set_psbox_height_and_weight(){
  bool dbThis=false;
  if(phaseSpaceFrac() <= 0) return;
  
  double rat = calc_pick_ps_prob()/phaseSpaceFrac();
  double w=1;
  if(rat > 0) w=rat;
  _psbox.weight() *= w;
  _psbox.height() /= w;

  if(dbThis){
    cout << "h " << _psbox.height() 
	 << ", w " << _psbox.weight()
	 << endl;
  }
}

double DalitzBWBoxSet::calc_pick_ps_prob() const{
  bool dbThis=false;
  if(_phaseSpaceFrac <= 0) return 0;
  double returnVal= _psbox.volume()/(_psbox.volume() + VolumeSum());
  if(dbThis){
    cout << "calc_pick_ps_prob() = "
	 <<  _psbox.volume() << " / " << "(" 
	 << _psbox.volume() << " + " <<  VolumeSum() << ")"
	 << " = " << returnVal << endl;
  }
  return returnVal;
}
double DalitzBWBoxSet::pick_ps_prob(){
  bool dbThis=false;
  if(_phaseSpaceFrac <= 0) return 0;
  if(_pick_ps_prob < 0){
    _pick_ps_prob = calc_pick_ps_prob();
    if(dbThis){
      cout << " DalitzBWBoxSet::pick_ps_prob() = " << _pick_ps_prob << endl;
    }
  }
  return _pick_ps_prob;  
}

void DalitzBWBoxSet::setup_psbox(){
  _psbox.setPattern(this->begin()->pattern());

  _psbox.height()=phaseSpaceFrac()*heightSum()/(1.0-phaseSpaceFrac());
  _psbox.weight()=1.0;
  for(int i=0; i < 100; i++){
    set_psbox_height_and_weight();
  }
}

void DalitzBWBoxSet::getReady(){
  //  checkIntegration();
  bool dbThis=false;
  if(this->empty()) return;
  setup_psbox();

  makeVolumeProbIntervals();
  _eventPtrList.clear();

  // checkIntegration(); // for debug only

  if(dbThis){
    cout << "DalitzBWBoxSet::getReady() - got ready, result is this: "
	 << (*this) << "\n ======= OK, no? ====== " << endl;
  }
  _ready=true;
  //  am_I_generating_what_I_think_I_am_generating(1000000); // for debug only

}

void DalitzBWBoxSet::findMax(){
  bool dbThis=false;

  time_t startTime = time(0);
  // int maxEvents = 100000;
  int maxEvents = 60000; // memory
  int maxTries = 9;

  bool speedupabit=true;
  if(speedupabit){
    maxEvents=50000;
    maxTries = 3;
  }
  bool speedupalot=false;
  if(speedupalot){
    maxEvents=30000;
    maxTries = 2;
  }
  bool instant=false; // set true for very fast & very dirty testing jobs etc;
  if(instant){
    maxEvents=5000;
    maxTries = 1;
  }
  //int maxEvents = 100000;
  //  maxEvents = 10000;

  cout << "DalitzBWBoxSet::findMax making starter set with "
       << maxEvents << " events." << endl;

  int numTries=0;
  _maxWeightEstimate=-9999; // estimated max from pareto

  while(_maxWeightEstimate < 0 || 
	(_maxWeightEstimate > _maxWeightInSample*2 
	&& numTries < maxTries 
	&& _eventPtrList.size() < 1000000)
    ){
    numTries++;
    cout << " DalitzBWBoxSet::findMax(): iteration number " << numTries 
	 << " for estimated max of parent " << _maxWeightEstimate
	 << " and max of current sample: " << _maxWeightInSample
	 << endl;
    for(int i=0; i < maxEvents; i++){
      counted_ptr<DalitzEvent> evt(makeWeightedEventForOwner());

      unsigned int printEvery = 100000;
      if(i <= 10) printEvery=10;
      else if(i <= 100) printEvery=100;
      else if(i <= 1000) printEvery=1000;
      else if(i <= 100000) printEvery=10000;
      else if(i <= 1000000) printEvery=500000;
      
      if(i < 1 || i%printEvery == 0){
	cout << "DalitzBWBoxSet::findMax() "
	     << "made event number " << i 
	     << "\t (" << difftime(time(0), startTime) << " sec)"
	     << endl;
      }
      if(dbThis && (0 != evt)) {
	cout << "DalitzBWBoxSet::findMax(): made event with weight "
	     << evt->getWeight() << endl;
      }
      if(0 != evt) _eventPtrList.Add(evt);
    }

    cout << "DalitzBWBoxSet::findMax() starter set took "
	 << difftime(time(0), startTime) << " s." << endl;

    _maxWeightEstimate = findMaxInList(_maxWeightInSample);
    //    _maxWeightEstimate = 60; // debug only june 2011
    //    break;  // debug only june 2011
  }

  if(_maxWeightEstimate > _maxWeightInSample*4
     || _maxWeightEstimate < _maxWeightInSample){
    _maxWeightEstimate = _maxWeightInSample*4;
  }
  //  _maxWeightEstimate = 60; // debug only june 2011

  if(dbThis){
    cout << "DalitzBWBoxSet::findMax(): I have now "
	 << _eventPtrList.size()
	 << " events in the eventPtrList." << endl;
    if( _eventPtrList.size() > 0){
      cout << "The first one is: " << _eventPtrList[0] << endl;
    }
  }

  //if(dbThis)_eventPtrList.save("eventListBeforeUnweighting.root");

  ReturnWeight w;
  justThrowAwayData(_maxWeightEstimate, &w);

  if(dbThis){
    cout << "DalitzBWBoxSet::findMax(): AFTER unweighting I have "
	 << _eventPtrList.size()
	 << " events in the eventPtrList." << endl;
    if( _eventPtrList.size() > 0){
      cout << "The first unweighted event is: " << _eventPtrList[0] << endl;
    }
  }



  /*
  _eventPtrList.findMaxAndThrowAwayData(_maxWeightEstimate, &w);
  */

  // we've un-weighted them, need to set weight correctly...
  for(unsigned int i=0; i < _eventPtrList.size(); i++){
    (_eventPtrList[i]).setWeight(1);
  }

  cout << "DalitzBWBoxSet::findMax() after throw away I have "
       << _eventPtrList.size() << " events left. Maximum weight estimate is: "
       << _maxWeightEstimate << endl;

  cout << "DalitzBWBoxSet::findMax() done after "
       << difftime(time(0), startTime) << " s." << endl;
  return;
}

double DalitzBWBoxSet::findMaxInList(double& sampleMax){
  bool dbThis=false;


  time_t startTime = time(0);
  if(0 == _eventPtrList.size()) return -9999;

  std::vector<double> vals;
  vals.resize(_eventPtrList.size());
  sampleMax=-9999;
  for(unsigned int i=0; i < _eventPtrList.size(); i++){
    DalitzEvent& evt(_eventPtrList[i]);

    double w = evt.getWeight();
    double d=w;

    if(dbThis) cout << "w = " << w << endl;

    if(d > sampleMax || 0 == i) sampleMax=d;
    vals[i] = d;

    
    unsigned int printEvery = _eventPtrList.size()/10;
    if(printEvery <=0) printEvery = 5;
    if(i <= 2) printEvery=1;
    else if(i <= 200) printEvery=100;
    
    if(i < 5 || 0 == i%(printEvery)){
      std::cout << "DalitzBWBoxSet::findMaxInList() calculated "
		<< "_ampSum for event " 
		<< i 
		<< ", its value is " << d
		<< std::endl;
      double deltaT = difftime(time(0), startTime);
      std::cout << " DalitzBWBoxSet::findMaxInList()this took " 
		<< deltaT << " s";
      if(deltaT > 0){
	std::cout << "; rate (before throwing away) = " 
		  << i/deltaT
		  << " evts/s";
      }
      std::cout << std::endl;
    }
  };
  
  
  double epsilon = 0.2;
  //    sampleMax = sampleMax + fabs(sampleMax * epsilon);
  
  double CL = 1.0 - 1./(_eventPtrList.size()*10000);
  std::cout << "sampleMax = " << sampleMax << std::endl;

  double maxValue = generalisedPareto_estimateMaximum(vals, CL);
  std::cout << " maxValue after " << maxValue << std::endl;
  maxValue *= 1.0 + epsilon;
  std::cout << "DalitzBWBoxSet::findMaxInList():: Now added " 
	    << epsilon * 100 << "% for safety. Returning "
	    << maxValue << std::endl;
 
  return maxValue;
}

int DalitzBWBoxSet::justThrowAwayData(double maxValue
				      , IReturnRealForEvent<IDalitzEvent>* amps
				      ){
  //  bool dbThis=true;
  std::cout << "EventPtrList::justThrowAwayData:"
	    << " before throwing away data, my size is " 
	    << _eventPtrList.size() << std::endl;
  
  time_t startTime = time(0);
  
  int rememberSize = _eventPtrList.size();
  unsigned int counter=0;
  
  DalitzEventPtrList newList;
  
  for(unsigned int i=0; i < _eventPtrList.size(); i++){
    double d=amps->RealVal(_eventPtrList[i]);
    if(_rnd->Rndm()*maxValue < d){
      newList.Add( _eventPtrList[counter] );
    }
    
    unsigned int printEvery = size()/10;
    if(printEvery <=0) printEvery = 5;
    //    if(counter <= 2) printEvery=1;
    else if(counter <= 200) printEvery=100;
    
    if(counter < 5 || 0 == counter%(printEvery)){
      std::cout << " amps for event " 
		<< counter 
		<< ": " << d
		<< ".  "  << newList.size() 
		<< " events passed ";
      double deltaT = difftime(time(0), startTime);
      std::cout << ". Took " << deltaT << " s";
      
      if(deltaT > 0){
	std::cout << "; rate (before/after throwing away) = " 
		  << counter/deltaT
		  << " / " << newList.size()/deltaT
		  << " evts/s";	  
      }
      std::cout << std::endl;
    }
    counter++;
  };
  
  _eventPtrList = newList;
  std::cout << "now my size has changed to " << _eventPtrList.size()
	    << std::endl;
  std::cout << " So the waste factor is ";
  if(size() > 0) std::cout << rememberSize/_eventPtrList.size();
  else std::cout << " infinity! - that's big!";
  std::cout << std::endl;
  
  
  double deltaTFinal = difftime(time(0), startTime);
  std::cout << " this took " << deltaTFinal/60.0 << " min" << std::endl;
  
  return _eventPtrList.size();
}



double DalitzBWBoxSet::heightSum() const{
  bool dbThis=false;

  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    if(dbThis)cout << " height number " << i << ") " 
		   << (*this)[i].height()
		   << endl;
    sum += (*this)[i].height();
  }

  if(dbThis)cout << " Volume sum: " << sum;
  return sum;
}
double DalitzBWBoxSet::VolumeSum() const{
  bool dbThis=false;

  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    if(dbThis)cout << " volume number " << i << ") " 
		   << (*this)[i].volume()
		   << endl;
    sum += (*this)[i].volume();
  }

  if(dbThis)cout << " Volume sum: " << sum;
  return sum;
}

void DalitzBWBoxSet::setUnWeightPs(bool doSo){
  for(unsigned int i=0; i< this->size(); i++){
    (*this)[i].setUnWeightPs(doSo);
  }
}

void DalitzBWBoxSet::makeVolumeProbIntervals(){
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
    //    (*this)[i].height() *= 1./totalVolume;
    sum += (*this)[i].volume()/totalVolume;
    if(dbThis){
      // cout << " with height " << (*this)[i].height();
      cout << " + " << (*this)[i].volume()/totalVolume;
      cout << " = \t volume probs [" << i <<"] = " << sum << endl;
    }
    _volumeProbs[i] = sum;
  }
}

int DalitzBWBoxSet::pickRandomVolume(){
  if(! ready()) getReady();

  if(_volumeProbs.size() != this->size()){
    makeVolumeProbIntervals();
  }

  double rndNumber = _rnd->Rndm();
  for(unsigned int i=0; i< _volumeProbs.size(); i++){
    if(_volumeProbs[i] > rndNumber) return i;
  }
  cout << "WARNING in DalitzBWBoxSet::pickRandomVolume():"
       << " How odd - should never have gotten here."
       << " rndNumber = " << rndNumber
       << ", _volumeProbs[this->size()-1] = "
       << _volumeProbs[this->size()-1]
       << endl;
  return this->size()-1;
}


counted_ptr<DalitzEvent> DalitzBWBoxSet::phaseSpaceEvent(){
  if(this->empty()) return counted_ptr<DalitzEvent>(0);
  if(! ready()) getReady();
  return _psbox.makeEventForOwner();
}

counted_ptr<DalitzEvent> DalitzBWBoxSet::tryEventForOwner(){
  bool dbThis=false;
  if(this->empty()){
    cout << "DalitzBWBoxSet::tryEventForOwner ERROR: "
	 << " you called me, but there are no boxes"
	 << "\n\t returning 0-ptr" << endl;
    return counted_ptr<DalitzEvent>(0);
  }
  if(! ready()) getReady();

  counted_ptr<DalitzEvent> evtPtr(0);
  if(_rnd->Rndm() < pick_ps_prob()){
    evtPtr = phaseSpaceEvent();
    if(dbThis){
      cout << "picked phaseSpace " << endl;
    }
  }else{
    int vol = pickRandomVolume();
    evtPtr = ((*this)[vol].tryEventForOwner());
    if(dbThis){
      cout << "picked volume number: " << vol << endl;
    }
  }
  if(dbThis && 0 != evtPtr){
    cout << "weight in DalitzBWBoxSet::tryEventForOwner() "
	 << evtPtr->getWeight() << endl;
  }
  if(0 != evtPtr){
    if(dbThis) {
      cout << "DalitzBWBoxSet::tryEventForOwner() calling:"
	   << " setGeneratorPdfRelativeToPhaseSpace(genValueNoPs(*evtPtr))"
	   << endl;
    }
    evtPtr->setGeneratorPdfRelativeToPhaseSpace(genValueNoPs(*evtPtr));
    if(dbThis) cout <<  " .. done that." << endl;
  }
  if(dbThis){
    cout << " DalitzBWBoxSet::tryEventForOwner(): Returning " << evtPtr << endl;
    if(0 != evtPtr){
      cout << " ... with weight " << evtPtr->getWeight() << endl;
    }
  }
  return evtPtr;
}
counted_ptr<DalitzEvent> DalitzBWBoxSet::makeEventForOwner(){
  int NTries = 0;
  return makeEventForOwner(NTries);
}
counted_ptr<DalitzEvent> DalitzBWBoxSet::makeEventForOwner(int& NTries){
  bool dbThis=false;
  static unsigned int Ncalls=0;
  static unsigned int printEveryNthCall = 1000;

  Ncalls++;
  bool printThis = Ncalls < 3 || (0 == Ncalls%printEveryNthCall) || dbThis;
  
  NTries = 0;
  if(! _ready) getReady();
  if(_maxWeightEstimate < 0) findMax();
  if(dbThis) cout << "makeEventForOwner found max" << endl;
  if(_eventPtrList.size() > 0){
    if(dbThis) cout << "popping Event" << endl;
    return popEventFromList();
  }
  if(dbThis) cout << "making new event" << endl;
  counted_ptr<DalitzEvent> evtPtr(0);
  
  int counter=0;
  int maxCount = 1000000;
  int countProblems = 0;
  do{
    int tempTries=0;
    counter++;
    if(counter > maxCount){
      cout << "DalitzBWBoxSet::makeEventForOwner tried "
	   << counter << " times unsuccessfully. Giving up"
	   << endl;
      return evtPtr;
    }
    evtPtr = makeWeightedEventForOwner(tempTries);
    NTries += tempTries;
    double w = -9999.0;
    if(0 != evtPtr) w = evtPtr->getWeight();
    if(w > _maxWeightInSample) _maxWeightInSample=w;
    if(w > _maxWeightEstimate){
      countProblems++;
      cout << "DalitzBWBoxSet::makeEventForOwner ERROR!!! "
	   << " for the " << countProblems << " th time. "
	   << "\n\t This event's weight > max Weight: "
	   << evtPtr->getWeight() << " > " << _maxWeightEstimate
	   << endl;
      _maxWeightEstimate = evtPtr->getWeight()*1.3;
      cout << "\n\t...increased _maxWeightEstimate to: " 
	   << _maxWeightEstimate
	   << endl;
    }
  }while(0 == evtPtr || _rnd->Rndm()*_maxWeightEstimate > evtPtr->getWeight());
  
  if(printThis){
    cout << "DalitzBWBoxSet::makeEventForOwner INFO after "
	 << Ncalls << " calls."
	 << " Estimate of max weight " << _maxWeightEstimate
	 << " max weight in sample so far " << _maxWeightInSample
	 << "." << endl;
  }

  if(0 != evtPtr) evtPtr->setWeight(1);
  return evtPtr;
}

counted_ptr<IDalitzEvent> DalitzBWBoxSet::newEvent(){
  bool dbThis=false;
  if(! dbThis){
    counted_ptr<IDalitzEvent> returnThis(makeWeightedEventForOwner());
    return returnThis;
  } else {
    counted_ptr<IDalitzEvent> returnThis(tryEventForOwner());
    return returnThis;
  }
}
counted_ptr<IDalitzEvent> DalitzBWBoxSet::newUnweightedEvent(){
  counted_ptr<IDalitzEvent> returnThis(makeEventForOwner());
  return returnThis;
}

counted_ptr<DalitzEvent> DalitzBWBoxSet::popEventFromList(){
  if(_eventPtrList.empty()){
    return counted_ptr<DalitzEvent>(0);
  }
  //  counted_ptr<DalitzEvent> evt(new DalitzEvent(_eventPtrList[_eventPtrList.size()-1]));
  //_eventPtrList.resize(_eventPtrList.size()-1);
  return _eventPtrList.popLastEventPtr();
}

counted_ptr<DalitzEvent> DalitzBWBoxSet::makeWeightedEventForOwner(){
  int NTries = 0;
  return makeWeightedEventForOwner(NTries);
}
counted_ptr<DalitzEvent> DalitzBWBoxSet::makeWeightedEventForOwner(int& NTries){
  bool dbThis=false;
  counted_ptr<DalitzEvent> ptr(makeWeightedApproxEventForOwner(NTries));
  if(! ptr) return ptr;
  double w = ptr->getWeight();
  if(dbThis) cout << "phase space weight " << w << endl;

  double full = fullPdf(*ptr);
  double gen  = genValueNoPs(*ptr);
  if(dbThis) cout << ", full weight " << w 
		  << " * " << full << " / " << gen << " = ";

  w *= full/gen;
  if(dbThis) cout << w << endl;

  ptr->setWeight(w);
  ptr->setGeneratorPdfRelativeToPhaseSpace(full);

  return ptr;
}

counted_ptr<DalitzEvent> DalitzBWBoxSet::makeWeightedApproxEventForOwner(){
  int NTries = 0;
  return makeWeightedApproxEventForOwner(NTries);
}
counted_ptr<DalitzEvent> DalitzBWBoxSet::makeWeightedApproxEventForOwner(int& NTries){
  //bool dbThis=false;
  counted_ptr<DalitzEvent> ptr(0);
  int maxTries = 10000000;
  NTries = 0;
  while((! ptr) && NTries < maxTries){
    ptr=tryEventForOwner();
    NTries++;
  }
  if(! ptr){ 
    cout << "ERROR in DalitzBWBoxSet::makeWeightedEventForOwner() "
	 << " failed to make event after " << maxTries
	 << " tries!"
	 << endl;
    return ptr;
  }
  return ptr;
}


double DalitzBWBoxSet::phaseSpaceIntegral()const{
  // mainly for debug.
  bool dbThis=false;

  if(_phaseSpaceIntegral > 0) return _phaseSpaceIntegral;
  if(this->empty())return -9999;

  if(dbThis) cout << " calculating phaseSpaceIntegral " << endl;
  DalitzEventPattern pat = (*this)[0].pattern();
  DalitzBWBox box(pat);

  PhaseSpaceIntegral4body p4;
  cout << "compare to " << p4.getVal(pat) << endl;

  int maxEvents =  10000;
  int maxTries  =  10000 * maxEvents;
  int nTries=0;
  double sum=0, sumsq=0;
  for(int i=0; i < maxEvents && nTries < maxTries; i++){
    int triesForThisEvent=0;
    counted_ptr<DalitzEvent> ptr(box.makeEventForOwner(triesForThisEvent));
    nTries += triesForThisEvent;
    if(! ptr) continue;
    double val = ptr->phaseSpace();
    sum += val;
    sumsq += val*val;
    if(dbThis && (0 == i%1000 || i < 100)){
      cout << " i = " << i << ", nTries = " << nTries 
	   << " val = " << val << " sum " << sum << endl;
    }
  }
  double mean = sum/((double)nTries);
  double meanOfSq = sumsq/((double)nTries);
  double variance = meanOfSq - mean*mean;
  double error = sqrt(variance)/sqrt((double)nTries);

  _phaseSpaceIntegral = sum/((double)nTries) * box.area().size();
  //  _phaseSpaceIntegral = sum/((double)maxEvents);// * box.volume();
  //  double psVolume = box.volume() * ((double)maxEvents)/((double)nTries);
  //double psVolume = box.volume();
  //_phaseSpaceIntegral = sum/((double)maxEvents);// * psVolume;
  cout << "phaseSpaceIntegral: returning " << _phaseSpaceIntegral
       << " relativeError: " << error/mean * 100 << "%" << endl;

  return _phaseSpaceIntegral;
}

double DalitzBWBoxSet::genValueNoPs(const DalitzEvent& evt)const{
  bool dbThis=false;
  double sum=0;
  if(dbThis){
    cout << "DalitzBWBoxSet::genValueNoPs called with " << evt
	 <<  endl;
  }
  
  if(this->empty()) return 0;

  for(unsigned int i=0; i<this->size(); i++){
    double val = (*this)[i].genValue(evt);
    if(dbThis){
      cout << "DalitzBWBoxSet::genValueNoPs adding " << (*this)[i].name()
	   << " with " << val << endl;
    }
    sum += val;
  }
  if(phaseSpaceFrac() > 0)sum += _psbox.genValue();

  if(dbThis){
    cout << "DalitzBWBoxSet::genValueNoPs returning " << sum << endl;
  }
  return sum;
}
double DalitzBWBoxSet::genValuePs(const DalitzEvent& evt)const{
  //bool dbThis=false;
  return evt.phaseSpace()/phaseSpaceIntegral();
}


double DalitzBWBoxSet::genValueWithLoop(DalitzEvent& evt)const{
  // for debug only
  //bool dbThis=false;
  double sum=0;
  for(int i=0; i < evt.numPermutations(); i++){
    evt.setPermutationIndex(i);
    sum += genValue(evt);
  }
  evt.setPermutationIndex(0);
  sum /= ((double) evt.numPermutations());
  return sum;
}

double DalitzBWBoxSet::genValue(const DalitzEvent& evt)const{
  bool dbThis=false;
  double sum=0;
  sum = genValueNoPs(evt) * (1.0 - phaseSpaceFrac());

  if(dbThis) cout <<  "DalitzBWBoxSet::genValue sum before phase space " 
		  << sum << endl;
  double ps = phaseSpaceFrac()*genValuePs(evt);
  if(dbThis) cout <<  "DalitzBWBoxSet::genValue adding ps = " 
		  << ps << endl;
  sum += ps;
  if(dbThis) cout <<  "DalitzBWBoxSet::genValue sum after phase space " 
		  << sum << endl;
   return sum;
}
void DalitzBWBoxSet::print(ostream& os) const{
  for(unsigned int i=0; i < this->size(); i++){
    os << " box " << i << " )" << (*this)[i] << "\n";
  }
  return;
}

ostream& operator<<(ostream& os, const DalitzBWBoxSet& box){
  box.print(os);
  return os;
}
//
