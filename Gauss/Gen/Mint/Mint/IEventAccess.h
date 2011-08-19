#ifndef IEVENTACCESS_ABC_HH
#define IEVENTACCESS_ABC_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/Utils.h"
#include "Mint/IBasicEventAccess.h"
//#include "Mint/IEventList.h"
//#include "Mint/ILoopable.h"

namespace MINT{

// forward declaration of IEventList
template<typename EVENT_TYPE> class IEventList;

template<typename EVENT_TYPE>
class IEventAccess: virtual public IBasicEventAccess<EVENT_TYPE>{
 public:

  virtual void setEvent(EVENT_TYPE *erptr)=0;

  virtual void setEventRecord(IEventList<EVENT_TYPE> *erptr
			      , OWNERSHIP ons=CALLER_KEEPS_OWNERSHIP)=0;
  virtual void resetEventRecord()=0; // to whatever it was set to previously

  virtual void setDaddy(IBasicEventAccess<EVENT_TYPE> *ia)=0;

  // the following 4 are from IBasicEventAccess:
  virtual const IEventList<EVENT_TYPE> * getEventRecord() const=0;
  virtual IEventList<EVENT_TYPE> * getEventRecord()=0;
  virtual const EVENT_TYPE * getEvent() const=0;
  virtual EVENT_TYPE * getEvent()=0;

  virtual ~IEventAccess(){}
};
}//namespace MINT

#endif
//


