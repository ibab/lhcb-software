#ifndef IEVENTLOOPACCESS_HH
#define IEVENTLOOPACCESS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/IEventAccess.h"

namespace MINT{

template<typename EVENT_TYPE>
class IEventLoop: virtual public IEventAccess<EVENT_TYPE>{

 public:
  /* in IEventAccess
  virtual void setEventRecord(IEventList<EVENT_TYPE>* erptr)=0;
  virtual const IEventList<EVENT_TYPE>* getEventRecord() const=0;
  virtual EVENT_TYPE* getEvent()=0;
  virtual const EVENT_TYPE* getEvent() const=0;
  */

  virtual bool getFirstEvent()=0;
  virtual bool getNextEvent()=0;

};
}//namespace MINT
#endif
//


