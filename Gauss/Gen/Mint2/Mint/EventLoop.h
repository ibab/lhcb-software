#ifndef EVENTLOOP_HH
#define EVENTLOOP_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <iostream>
#include "Mint/IEventList.h"
#include "Mint/IEventAccess.h"
#include "Mint/IEventLoop.h"
#include "Mint/EventAccess.h"

namespace MINT{

template<typename EVENT_TYPE>
class EventLoop
: public EventAccess<EVENT_TYPE>
, virtual public IEventLoop<EVENT_TYPE>
{
 public:
  IEventList<EVENT_TYPE>* getEventRecord(){
    return EventAccess<EVENT_TYPE>::getEventRecord();
  }
  const IEventList<EVENT_TYPE>* getEventRecord() const{
    return EventAccess<EVENT_TYPE>::getEventRecord();
  }
  bool getFirstEvent(){
    if(0 == getEventRecord()){
      std::cout << "ERROR in EventLoop::getFirstEvent()"
		<< " no Access to Event Record!"
		<< std::endl;
      return false;
    }
    return getEventRecord()->Start();
  }
  bool getNextEvent(){
    if(0 == getEventRecord()){
      std::cout << "ERROR in EventLoop::getNextEvent()"
		<< " no Access to Event Record!"
		<< std::endl;
      return false;
    }
    return getEventRecord()->Next();
  }

  EventLoop(IEventAccess<EVENT_TYPE>* erptr = 0)
    : EventAccess<EVENT_TYPE>(erptr){}

  EventLoop(IEventList<EVENT_TYPE>* erptr = 0)
    : EventAccess<EVENT_TYPE>(erptr){}

  EventLoop(const EventLoop<EVENT_TYPE>& other)
    : EventAccess<EVENT_TYPE>(other)
    {}
};

}//namespace MINT
#endif
//
