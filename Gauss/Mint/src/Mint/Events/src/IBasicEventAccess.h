#ifndef IBASICEVENTACCESS_ABC_HH
#define IBASICEVENTACCESS_ABC_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

namespace MINT{

// forward declaration of IEventList
template<typename EVENT_TYPE> class IEventList;

template<typename EVENT_TYPE>
class IBasicEventAccess{
 public:
  IBasicEventAccess(){}

  virtual const IEventList<EVENT_TYPE> * getEventRecord() const=0;
  virtual IEventList<EVENT_TYPE> * getEventRecord()=0;
  virtual const EVENT_TYPE * getEvent() const=0;
  virtual EVENT_TYPE * getEvent()=0;

  virtual ~IBasicEventAccess(){}
};

}//namespace MINT
#endif
//
