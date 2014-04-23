#ifndef DALITZEVENTACCESS_HH
#define DALITZEVENTACCESS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "EventAccess.h"
#include "counted_ptr.h"
#include "DalitzEvent.h"
#include "DalitzEventPattern.h"
#include "IDalitzEvent.h"
#include "IDalitzEventAccess.h"

class DalitzEventAccess : public MINT::EventAccess<IDalitzEvent>
, virtual public IDalitzEventAccess
{
  MINT::counted_ptr<IDalitzEvent> dummyEventPtr;
 public:
  DalitzEventAccess(MINT::IEventAccess<IDalitzEvent>* dad)
    : MINT::EventAccess<IDalitzEvent>(dad)
    , dummyEventPtr(0)
    {}
  
 DalitzEventAccess(MINT::IEventList<IDalitzEvent>* elist)
   : MINT::EventAccess<IDalitzEvent>(elist)
    , dummyEventPtr(0)
    {}
  
  DalitzEventAccess(const DalitzEventPattern& pat)
    : MINT::EventAccess<IDalitzEvent>((MINT::IEventAccess<IDalitzEvent>*)0)
    , dummyEventPtr(new DalitzEvent(pat))
    {
      this->setEvent(dummyEventPtr.get());
    }
  
  DalitzEventAccess(const DalitzEventAccess& other)
    : MINT::IBasicEventAccess<IDalitzEvent>()
    , MINT::IEventAccess<IDalitzEvent>()
    , IDalitzEventAccess()
    , MINT::EventAccess<IDalitzEvent>(other)
    , dummyEventPtr(other.dummyEventPtr)
    {
      if(0 != dummyEventPtr) setEventFront(dummyEventPtr.get());
    }

  DalitzEventAccess& operator=(const DalitzEventAccess& other){
    if(&other == this) return *this;

    MINT::EventAccess<IDalitzEvent>(*this)
      = (MINT::EventAccess<IDalitzEvent>) other;

    dummyEventPtr = other.dummyEventPtr;
    if(0 != dummyEventPtr) setEventFront(dummyEventPtr.get());
    return *this;
  }

  virtual ~DalitzEventAccess(){}
};

#endif
//
