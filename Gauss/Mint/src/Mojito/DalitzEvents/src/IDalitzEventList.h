#ifndef IDATLIZEVENTLIST_HH
#define IDATLIZEVENTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "IEventList.h"
#include "IDalitzEvent.h"
#include "DalitzEvent.h"

#include "counted_ptr.h"

typedef MINT::IEventList<IDalitzEvent> IDalitzEventList;

/*
class IDalitzEventList : virtual public MINT::IEventList<IDalitzEvent>{

 public:
  virtual bool Add(const DalitzEvent& evt) = 0;
};
*/

#endif
//
