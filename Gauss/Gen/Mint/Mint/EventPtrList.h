#ifndef EVENT_PTR_LIST_HH
#define EVENT_PTR_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include "TRandom.h"

#include "Mint/IEventList.h"
#include "Mint/IEventAccess.h"
//#include "Mint/EventAccess.h"
#include "Mint/IReturnReal.h"
#include "Mint/IGetRealEvent.h"

#include "Mint/GeneralisedPareto.h"

#include "Mint/counted_ptr.h"

namespace MINT{

template<typename RETURN_TYPE, typename STORE_TYPE = RETURN_TYPE>
class EventPtrList 
//: virtual public IEventAccess<RETURN_TYPE>
//, public IEventAccess<RETURN_TYPE>
  : virtual public IEventList<RETURN_TYPE >
  , public std::vector<counted_ptr<STORE_TYPE> > 
    {
  
  protected:
  bool _initialised;
  typename std::vector<counted_ptr<STORE_TYPE> >::iterator currentPosition;
  typename std::vector<counted_ptr<STORE_TYPE> >::iterator nextPosition;

  bool init(){
    return (_initialised=Start());
  }
  public:
  EventPtrList()
  {
    init();
  }

  EventPtrList(const STORE_TYPE& evt)
  {
    init();
    Add(evt);
    Start();
  }

  EventPtrList(const EventPtrList<RETURN_TYPE, STORE_TYPE>& other)
  : ILoopable()
  , IBasicEventAccess<RETURN_TYPE>()
  , IEventList<RETURN_TYPE>()
  , std::vector<counted_ptr<STORE_TYPE> >(other)
  {
    init();
  }
  EventPtrList<RETURN_TYPE, STORE_TYPE>& 
  operator=(const EventPtrList<RETURN_TYPE, STORE_TYPE>& other){
    if(this == &other) return *this;
    this->clear();
    for(unsigned int i=0; i<other.size(); i++){
      this->push_back(other[i]);
    }
    /*
    this->_recordCollection = other._recordCollection;
    this->_recordCollection.pop_front();
    this->_recordCollection.push_front((IEventList< RETURN_TYPE >*) this);
    */
    _initialised=false;
    this->init();
    return *this;
  }

  virtual unsigned int size() const{
    return std::vector<counted_ptr<STORE_TYPE> >::size();
  }
  virtual bool Start(){
    if(this->empty()) return false;
    currentPosition = nextPosition = this->begin();
    //    if(nextPosition != this->end()) nextPosition++;
    // this way, (i.e above line commented out)
    // the 1st call to Next() gives 1st event.
    //else return false;
    return true;
  }
  virtual bool Next(){ // 1st call to Next gives first event!
    // a bit complicated, but insures save deleting
    if(this->empty()) return false;
    if(! _initialised) init();

    currentPosition = nextPosition;
    if(currentPosition == this->end()){
      Start();
      return false;
    }
    nextPosition++;
    return true;
  }
  virtual bool curryCheck(){
    if(this->empty())return 0;
    if(! _initialised) init();
    if(currentPosition == this->end()) Start();
    if(currentPosition == this->end()) return 0;
    return true;
  }
  virtual RETURN_TYPE* getEvent(){
    if(! curryCheck()) return 0;
    return (RETURN_TYPE*) currentPosition->get();
  }
  virtual const RETURN_TYPE* getEvent() const{
    if(this->empty())return 0;
    if(! _initialised) return this->begin()->get();
    if(currentPosition == this->end()) return this->begin()->get();
    return (const RETURN_TYPE*) currentPosition->get();
  }
  
  virtual RETURN_TYPE* getREvent(unsigned int i){
    if(i >= size()) return 0;
    return ((*this)[i]).get();
  }
  virtual const RETURN_TYPE* getREvent(unsigned int i) const{
    if(i >= size()) return 0;
    return ((*this)[i]).get();
  }
  virtual MINT::counted_ptr<STORE_TYPE> popLastEvent(){
    if(this->empty()) return MINT::counted_ptr<STORE_TYPE>(0);
    MINT::counted_ptr<STORE_TYPE> evt((*this)[this->size()-1]);
    this->resize(this->size() - 1);
    return evt;
  }

  virtual bool Delete(){
    if(this->empty()) return false;
    if(currentPosition == this->end()) return false;
    nextPosition=this->erase(currentPosition);
    return true;
  }

  /*
  virtual bool Add(const STORE_TYPE& evt){
    counted_ptr<STORE_TYPE> copy (new STORE_TYPE (evt));
    this->push_back(copy);
    return true;
  }
  */
 
  virtual bool Add(const MINT::counted_ptr<STORE_TYPE>& evt){
    this->push_back(evt);
    return true;
  }

  virtual bool Add(const EventPtrList<RETURN_TYPE, STORE_TYPE>& otherList){
    if(otherList.empty()) return false;
    for(unsigned int i=0; i < otherList.size(); i++){
      Add(otherList[i]);
    }
    return true;
  }
  class iterator : public std::vector<counted_ptr<STORE_TYPE> >::iterator{
    public:
    iterator() : std::vector<counted_ptr<STORE_TYPE> >::iterator(){}
    iterator(const typename std::vector<counted_ptr<STORE_TYPE> >::iterator& other) 
    : std::vector<counted_ptr<STORE_TYPE> >::iterator(other){}
    iterator(const iterator& other) 
    : std::vector<counted_ptr<STORE_TYPE> >::iterator(other){}
 };
  class const_iterator : public std::vector<counted_ptr<STORE_TYPE> >::const_iterator{
    public:
    const_iterator() : std::vector<counted_ptr<STORE_TYPE> >::const_iterator(){}
    const_iterator(const typename std::vector<counted_ptr<STORE_TYPE> >::iterator& other) 
      : std::vector<counted_ptr<STORE_TYPE> >::const_iterator(other){}
    const_iterator(const const_iterator& other) 
      : std::vector<counted_ptr<STORE_TYPE> >::const_iterator(other){}
  };


  double getMax(IReturnReal* amps){
    double max=-1;
    int counter=0;
    this->Start();
    while(this->Next()){// 1st call to next gives 1st event
      double d=amps->RealVal();
      if(d > max || 0 == counter) max=d;
      counter++;
    };
    return max;
  }
 
  double getMin(IReturnReal* amps){
    double min = +9999;
    int counter=0;
    this->Start();
    while(this->Next()){// 1st call to next gives 1st event
      double d=amps->RealVal();
      if(d < min || 0 == counter) min=d;
      counter++;
    }
    return min;
  }
  
  int findMaxAndThrowAwayData(double& maxValue
			      , IGetRealEvent<RETURN_TYPE>* amps
			      , int Nevents  = -1
			      , TRandom* rnd = gRandom)
  {
    //    bool dbThis=true;
    std::cout << "EventPtrList::findMaxAndThrowAwayData:"
    << " before throwing away data, my size is " 
    << this->size() << std::endl;
    
    time_t startTime = time(0);
    
    int rememberSize = this->size();
    unsigned int counter=0;

    std::vector<double> vals;
    vals.resize(this->size());
    this->Start();
    amps->setEventRecord(this);
    while(this->Next()){// 1st call to next gives 1st event
      double d=amps->RealVal();
      if(d > maxValue || 0 == counter) maxValue=d;
      vals[counter] = d;

      unsigned int printEvery = size()/10;
      if(printEvery <=0) printEvery = 5;
      if(counter <= 2) printEvery=1;
      else if(counter <= 200) printEvery=100;

      if(counter < 5 || 0 == counter%(printEvery)){
	std::cout << " calculated amps for event " 
		  << counter 
		  << ", its value is " << d
		  << std::endl;
	double deltaT = difftime(time(0), startTime);
	std::cout << " this took " << deltaT << " s";
	if(deltaT > 0){
	  std::cout << "; rate (before throwing away) = " 
		    << counter/deltaT
		    << " evts/s";
	}
	std::cout << std::endl;
      }
      counter++;
    };
    amps->resetEventRecord();


    double epsilon = 0.2;
    //    maxValue = maxValue + fabs(maxValue * epsilon);

    double CL = 1.0 - 1./(this->size()*10000);
    std::cout << "maxValue before = " << maxValue << std::endl;
    maxValue = generalisedPareto_estimateMaximum(vals, CL);
    std::cout << " maxValue after " << maxValue << std::endl;
    maxValue *= 1.0 + epsilon;
    std::cout << "Now added " << epsilon * 100 << "% for safety "
              << maxValue << std::endl;

    EventPtrList<RETURN_TYPE, STORE_TYPE> newList;
    
    for(counter=0; counter<this->size(); counter++){
      double d=vals[counter]; 
      if(rnd->Rndm()*maxValue < d){
	newList.Add( (*this)[counter] );
      }
      unsigned int printEvery = size()/2;
      if(printEvery <=0) printEvery = 2;

      if(counter < 2 || 0 == counter%(printEvery)){
	std::cout << " remembering amps for event " 
		  << counter 
		  << ", its value is " << d
		  << std::endl;
      }
      if(Nevents > 0 && (int)newList.size() >= Nevents) break;
    }
    (*this) = newList;
    std::cout << "now my size has changed to " << this->size()
    << std::endl;
    std::cout << " So the waste factor is ";
    if(size() > 0) std::cout << rememberSize/this->size();
    else std::cout << " infinity! - that's big!";
    std::cout << std::endl;


    double deltaTFinal = difftime(time(0), startTime);
    std::cout << " this took " << deltaTFinal/60.0 << " min";
    if(deltaTFinal > 0){
      std::cout << " rate = " << (this->size()/deltaTFinal) << " evts/s"
		<< " or " << (this->size()/deltaTFinal) *60 << " evts/m"
		<< " or " << (this->size()/deltaTFinal) *60.*60./1000. 
		<< "k evts/h";
    }
    std::cout << std::endl;

    std::cout << " ---------------\n " << std::endl;
    this->Start();
    return this->size();
  }
  int justThrowAwayData(double maxValue
			      , IGetRealEvent<RETURN_TYPE>* amps
			      , TRandom* rnd = gRandom)
  {
    //    bool dbThis=true;
    std::cout << "EventPtrList::justThrowAwayData:"
    << " before throwing away data, my size is " 
    << this->size() << std::endl;
    
    time_t startTime = time(0);
    
    int rememberSize = this->size();
    unsigned int counter=0;

    EventPtrList<RETURN_TYPE, STORE_TYPE> newList;

    this->Start();
    amps->setEventRecord(this);
    while(this->Next()){// 1st call to next gives 1st event
      double d=amps->RealVal();
      if(rnd->Rndm()*maxValue < d){
	newList.Add( (*this)[counter] );
      }

      unsigned int printEvery = size()/10;
      if(printEvery <=0) printEvery = 5;
      if(counter <= 2) printEvery=1;
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
    amps->resetEventRecord();
    
    (*this) = newList;
    std::cout << "now my size has changed to " << this->size()
    << std::endl;
    std::cout << " So the waste factor is ";
    if(size() > 0) std::cout << rememberSize/this->size();
    else std::cout << " infinity! - that's big!";
    std::cout << std::endl;


    double deltaTFinal = difftime(time(0), startTime);
    std::cout << " this took " << deltaTFinal/60.0 << " min" << std::endl;

    this->Start();
    return this->size();
  }
  double findMax(double& maxInThisSample
		 , IGetRealEvent<RETURN_TYPE>* amps
		 )
  {
    //    bool dbThis=true;
    std::cout << "EventPtrList::findMax:" << std::endl;
    
    time_t startTime = time(0);
    
    //int rememberSize = this->size();
    unsigned int counter=0;
    std::vector<double> vals;
    vals.resize(this->size());

    this->Start();
    amps->setEventRecord(this);
    while(this->Next()){// 1st call to next gives 1st event
      double d=amps->RealVal();
      if(d > maxInThisSample || 0 == counter) maxInThisSample=d;
      vals[counter] = d;

      unsigned int printEvery = size()/10;
      if(printEvery <=0) printEvery = 5;
      if(counter <= 2) printEvery=1;
      else if(counter <= 200) printEvery=100;

      if(counter < 5 || 0 == counter%(printEvery)){
	std::cout << " calculated amps for event " 
		  << counter 
		  << ", its value is " << d
		  << std::endl;
	double deltaT = difftime(time(0), startTime);
	std::cout << " this took " << deltaT << " s";
	if(deltaT > 0){
	  std::cout << "; rate (before throwing away) = " 
		    << counter/deltaT
		    << " evts/s";
	}
	std::cout << std::endl;
      }
      counter++;
    };
    amps->resetEventRecord();


    double epsilon = 0.2;
    //    maxInThisSample = maxInThisSample + fabs(maxInThisSample * epsilon);

    double CL = 1.0 - 1./(this->size()*10000);
    std::cout << "maxValue in this sample = " << maxInThisSample << std::endl;
    double estimatedMaxOfParentSample = generalisedPareto_estimateMaximum(vals, CL);
    std::cout << " maxValue after " << estimatedMaxOfParentSample << std::endl;
    estimatedMaxOfParentSample *= 1.0 + epsilon;
    std::cout << "Now added " << epsilon * 100 << "% for safety "
              << estimatedMaxOfParentSample << std::endl;

    double deltaTFinal = difftime(time(0), startTime);
    std::cout << " this took " << deltaTFinal/60.0 << " min";
    if(deltaTFinal > 0){
      std::cout << " rate = " << (this->size()/deltaTFinal) << " evts/s"
		<< " or " << (this->size()/deltaTFinal) *60 << " evts/m"
		<< " or " << (this->size()/deltaTFinal) *60.*60./1000. 
		<< "k evts/h";
    }
    std::cout << std::endl;

    return estimatedMaxOfParentSample;
  }

  double findMax(IGetRealEvent<RETURN_TYPE>* amps
		 ){
    double maxInThisSample;
    return findMax(maxInThisSample, amps);
  }

  // for bw compatibility
  int throwAwayData(IGetRealEvent<RETURN_TYPE>* amps
		    , int Nevents=-1, TRandom* rnd=gRandom)
  {
    double maxValue = -9999;
    return findMaxAndThrowAwayData(maxValue, amps, Nevents, rnd);
  }

};

}//namespace MINT

#endif
//
