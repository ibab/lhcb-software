#ifndef IEVENTLISTB_HH
#define IEVENTLISTB_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/Utils.h"
#include "Mint/ILoopable.h"
#include "Mint/IBasicEventAccess.h"

/*
  loop like this:

  list.Start();
  while(list.Next()){
    do somthing;
  }

  i.e. 1st call to Next
  after Start gives first event.
*/

namespace MINT{

template<typename RETURN_TYPE>
class IEventList : virtual public ILoopable
, virtual public IBasicEventAccess<RETURN_TYPE>
{
 protected:
  IEventList(){}
 public:

  virtual unsigned int size() const=0;
  virtual RETURN_TYPE * getEvent()=0;
  virtual const RETURN_TYPE * getEvent() const=0;
  /* these are declared in ILoopable */
  virtual bool Next()   =0;
  virtual bool Start()  =0;
  //  virtual bool Delete()  =0;

  virtual const RETURN_TYPE * getREvent(unsigned int i) const=0;

  virtual ~IEventList(){}

  // in future I'll delete the next two, keep for now, for backw. compat:
  virtual RETURN_TYPE * currentEvent(){return getEvent();}
  virtual const RETURN_TYPE * currentEvent() const{return getEvent();}

  virtual const IEventList<RETURN_TYPE> * getEventRecord() const{return this;}
  virtual IEventList<RETURN_TYPE> * getEventRecord(){return this;}
};
}//namespace MINT
#endif
//
