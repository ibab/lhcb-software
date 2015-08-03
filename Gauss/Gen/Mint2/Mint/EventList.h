#ifndef EVENTLIST_HH
#define EVENTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include "TRandom.h"

#include "Mint/IEventList.h"

namespace MINT{
  
  template<typename EVENT_TYPE>
    class EventList 
    : virtual public IEventList<EVENT_TYPE >
    , public std::vector<EVENT_TYPE> 
    {
    public:
    EventList()
      : IEventList<EVENT_TYPE>()
	, std::vector<EVENT_TYPE>()
	{
	}
    EventList(const EventList<EVENT_TYPE>& other)
      : IEventList<EVENT_TYPE>()
	, std::vector<EVENT_TYPE>(other)
	{
	}
      EventList<EVENT_TYPE>& 
	operator=(const EventList<EVENT_TYPE>& other){
	if(this == &other) return *this;
	this->clear();
	for(unsigned int i=0; i<other.size(); i++){
	  this->push_back(other[i]);
	}
	return *this;
      }
      
      virtual const EVENT_TYPE& operator[](unsigned int i) const{
	return std::vector<EVENT_TYPE>::operator[](i);
      }

      virtual EVENT_TYPE& operator[](unsigned int i){
	return std::vector<EVENT_TYPE>::operator[](i);
      }
      
      EVENT_TYPE getEvent(unsigned int i)const{
	return (*this)[i];
      }

      virtual unsigned int size() const{
	return std::vector<EVENT_TYPE>::size();
      }
      
      virtual bool Add(const EVENT_TYPE & evt){
	this->push_back(evt);
	return true;
      }
      virtual bool Add(const EventList<EVENT_TYPE> & addList){
	for(unsigned int i = 0; i < addList.size(); i++){
	  this->push_back(addList[i]);
	}
	return true;
      }

  /*
  double getMax(IReturnReal<EVENT_TYPE>* amps){
    double max=-1;
    int counter=0;
    for(int i=0; i < this->size(); i++){
      double d=amps->RealVal((*this)[i]);
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
			      , IGetRealEvent<EVENT_TYPE>* amps
			      , int Nevents  = -1
			      , TRandom* rnd = gRandom)
  {
    //    bool dbThis=true;
    std::cout << "EventList::findMaxAndThrowAwayData:"
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

    EventList<EVENT_TYPE, EVENT_TYPE> newList;
    
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
			      , IGetRealEvent<EVENT_TYPE>* amps
			      , TRandom* rnd = gRandom)
  {
    //    bool dbThis=true;
    std::cout << "EventList::justThrowAwayData:"
    << " before throwing away data, my size is " 
    << this->size() << std::endl;
    
    time_t startTime = time(0);
    
    int rememberSize = this->size();
    unsigned int counter=0;

    EventList<EVENT_TYPE, EVENT_TYPE> newList;

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
		 , IGetRealEvent<EVENT_TYPE>* amps
		 )
  {
    //    bool dbThis=true;
    std::cout << "EventList::findMax:" << std::endl;
    
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

  double findMax(IGetRealEvent<EVENT_TYPE>* amps
		 ){
    double maxInThisSample;
    return findMax(maxInThisSample, amps);
  }

  // for bw compatibility
  int throwAwayData(IGetRealEvent<EVENT_TYPE>* amps
		    , int Nevents=-1, TRandom* rnd=gRandom)
  {
    double maxValue = -9999;
    return findMaxAndThrowAwayData(maxValue, amps, Nevents, rnd);
  }

  */
};

}//namespace MINT

#endif
//
