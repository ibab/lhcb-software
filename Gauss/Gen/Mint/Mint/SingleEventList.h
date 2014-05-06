#ifndef SINGLE_EVENT_LIST_HH
#define SINGLE_EVENT_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

// a simple wrapper, useful mainly in IEventAccess, to treat
// events like eventlists...

#include "Mint/IEventList.h"

namespace MINT{
template<typename RETURN_TYPE>
class SingleEventList : virtual public IEventList<RETURN_TYPE>{
  RETURN_TYPE* _eventPtr;
  bool _alternator;
 public:
  SingleEventList(RETURN_TYPE* ep=0){
    _eventPtr = ep;
    _alternator = true;
  }
  SingleEventList(const SingleEventList& other)
    : _eventPtr(other._eventPtr)
    , _alternator(other._alternator)
    {
    }
  virtual ~SingleEventList(){};

  virtual unsigned int size() const{
    if(0 == _eventPtr) return 0;
    else return 1;
  }
  virtual RETURN_TYPE * getEvent(){return _eventPtr;}
  virtual const RETURN_TYPE * getEvent() const{return _eventPtr;}
  virtual const RETURN_TYPE * getREvent(unsigned int i=0) const{
    if (i != 0) return 0;
    return _eventPtr;}
  /* these are declared in ILoopable */
  virtual bool Next(){_alternator = ! _alternator; return _alternator;}
  virtual bool Start(){_alternator = false; return true;}
  virtual bool Delete(){_eventPtr = 0; return true;}

  // in future I'll delete the next two, keep for now, for backw. compat:
  virtual RETURN_TYPE * currentEvent(){return getEvent();}
  virtual const RETURN_TYPE * currentEvent() const{return getEvent();}

  virtual const IEventList<RETURN_TYPE> * getEventRecord() const{return this;}
  virtual IEventList<RETURN_TYPE> * getEventRecord(){return this;}
};

}//namespace MINT

#endif
