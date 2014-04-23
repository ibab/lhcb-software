#ifndef IEVENTLISTB_HH
#define IEVENTLISTB_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/Utils.h"
#include "Mint/IMinimalEventList.h"

/*
   looks like array
*/

namespace MINT{

template<typename EVENT_TYPE>
  class IEventList : virtual public IMinimalEventList<EVENT_TYPE>
  // returns reference (for memory-resident lists)
{
 protected:
  IEventList(){}
 public:

  virtual const EVENT_TYPE& operator[](unsigned int i) const=0;
  virtual EVENT_TYPE& operator[](unsigned int i)=0;

  // included through IMinimalEventList:
  // virtual unsigned int size() const=0;
  // virtual EVENT_TYPE getEvent(unsigned int i) const=0;
  // virtual bool Add(const EVENT_TYPE & evt)=0;

};
}//namespace MINT
#endif
//
