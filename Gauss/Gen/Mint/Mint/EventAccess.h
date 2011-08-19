#ifndef EVENTACCESS_HH
#define EVENTACCESS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <iostream>
#include <deque>
#include <map>

#include "Mint/Utils.h"
#include "Mint/SingleEventList.h"
#include "Mint/EventList.h"
#include "Mint/IEventList.h"
#include "Mint/IBasicEventAccess.h"
#include "Mint/IEventAccess.h"

namespace MINT{

template<typename EVENT_TYPE>
class EventAccess : virtual public IEventAccess<EVENT_TYPE>{
 protected:
  std::deque< std::pair< IEventList<EVENT_TYPE>*, OWNERSHIP > > 
    _recordCollection;

  IBasicEventAccess<EVENT_TYPE>* _daddy;
  IEventList<EVENT_TYPE>* _tmpEventRecord;

  EventAccess()
    : IEventAccess<EVENT_TYPE>()
    , _recordCollection(0)
    , _daddy(0)
    , _tmpEventRecord(0)
    {}

  virtual void setEventFront(EVENT_TYPE* evtPtr){
    if(0 == evtPtr){
      setFirstEventRecord(0);
      return;
    }
    IEventList<EVENT_TYPE>* elPtr = new SingleEventList<EVENT_TYPE>(evtPtr);
    setFirstEventRecord(elPtr, CALLER_PASSES_OWNERSHIP);
  }
  virtual void setFirstEventRecord(IEventList<EVENT_TYPE>* erptr=0
				   , OWNERSHIP ons=CALLER_KEEPS_OWNERSHIP){
    _recordCollection.push_front(std::pair<IEventList<EVENT_TYPE>*, OWNERSHIP>( erptr, ons) );
  }

 public:
  const IEventList<EVENT_TYPE>* getEventRecord() const{
    if(_recordCollection.empty()){
      if(0 != _daddy) return _daddy->getEventRecord();
      return 0;
    }else{
      return _recordCollection.back().first;
    }
  }
  IEventList<EVENT_TYPE>* getEventRecord(){
    if(_recordCollection.empty()){
      if(0 != _daddy) return _daddy->getEventRecord();
      return 0;
    }else{
      return _recordCollection.back().first;
    }
  }
  virtual void setEvent(EVENT_TYPE* evtPtr){
    if(0 == evtPtr){
      setEventRecord(0);
      return;
    }
    IEventList<EVENT_TYPE>* elPtr = new SingleEventList<EVENT_TYPE>(evtPtr);
    setEventRecord(elPtr, CALLER_PASSES_OWNERSHIP);
  }

  virtual void setEventRecord(IEventList<EVENT_TYPE>* erptr=0
			      , OWNERSHIP ons=CALLER_KEEPS_OWNERSHIP){
    _recordCollection.push_back(std::pair<IEventList<EVENT_TYPE>*, OWNERSHIP>( erptr, ons) );
  }

  virtual void resetEventRecord(){
    if(_recordCollection.empty()) return;
    if(_recordCollection.back().second == CALLER_PASSES_OWNERSHIP){
      if(0 != _recordCollection.back().first){
	delete _recordCollection.back().first;
      }
    }
    _recordCollection.pop_back();
  }

  virtual void clearRecordCollection(){
    while(! _recordCollection.empty()) resetEventRecord();
  }

  virtual int getRecordCollectionSize() const{
    return _recordCollection.size();
  }

  virtual void setDaddy(IBasicEventAccess<EVENT_TYPE> *ia){
    clearRecordCollection();
    _daddy = ia;
  }

  EventAccess(IEventAccess<EVENT_TYPE>* dad)
    : IEventAccess<EVENT_TYPE>()
    , _recordCollection(0)
    , _daddy(dad)
    , _tmpEventRecord(0)
    {
    }
  EventAccess(IEventList<EVENT_TYPE>* elist)
    : IEventAccess<EVENT_TYPE>()
    , _recordCollection(0)
    , _daddy(0)
    , _tmpEventRecord(0)
    {
      setEventRecord(elist);
    }
  EventAccess(const EventAccess<EVENT_TYPE>& other)
    : IBasicEventAccess<EVENT_TYPE>()
    , IEventAccess<EVENT_TYPE>()
    , _recordCollection(0)
    , _daddy(other._daddy)
    , _tmpEventRecord(other._tmpEventRecord)
    {
      copyRecordCollection(other._recordCollection);
    }
  EventAccess<EVENT_TYPE>& operator=(const EventAccess<EVENT_TYPE>& other){
    if(&other == this) return *this;
    this->clearRecordCollection();
    copyRecordCollection(other._recordCollection);
    _daddy = other._daddy;
    _tmpEventRecord = other._tmpEventRecord;
    return *this;
  }

  void copyRecordCollection(const std::deque<std::pair<IEventList<EVENT_TYPE>*, OWNERSHIP > >& other){
    if(other.empty()) return;
    std::deque<std::pair<IEventList<EVENT_TYPE>*, OWNERSHIP > > copyOfOther(other);

    while(! copyOfOther.empty()){
      if(copyOfOther.back().second != CALLER_PASSES_OWNERSHIP){
	this->_recordCollection.push_back(copyOfOther.back());
      }
      copyOfOther.pop_back();
    }
  }

  virtual ~EventAccess(){
    clearRecordCollection();
  }

  EVENT_TYPE* getEvent(){
    //std::cout << "getting Event" << std::endl;
    if(0 == getEventRecord()) return 0;
    //std::cout << "there is a record" << std::endl;
    return getEventRecord()->getEvent();
  }
  const EVENT_TYPE* getEvent() const{
    //std::cout << "getting Event" << std::endl;
    if(0 == getEventRecord()) return 0;
    //std::cout << "there is a record" << std::endl;
    return getEventRecord()->getEvent();
  }
  int numEvents() const{
    //    std::cout << "getting Event" << std::endl;
    if(0 == getEventRecord()) return 0;
    return getEventRecord()->size();
  }
};

}//namespace MINT

#endif
//
