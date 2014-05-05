#ifndef IIDATLIZEVENTLIST_HH
#define IIDATLIZEVENTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/IEventList.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/counted_ptr.h"

//typedef MINT::IEventList<IDalitzEvent> IDalitzEventList;

class IIDalitzEventList : virtual public MINT::IEventList<IDalitzEvent>{

 public:
  virtual bool Add(MINT::counted_ptr<IDalitzEvent> evtPtr) = 0;
}
#endif
//
