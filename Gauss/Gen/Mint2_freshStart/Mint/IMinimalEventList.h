#ifndef IMINIMALEVENTLISTB_HH
#define IMINIMALEVENTLISTB_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/Utils.h"

/*
   looks like array
*/

namespace MINT{

template<typename EVENT_TYPE>
  class IMinimalEventList
{
 protected:
  IMinimalEventList(){}
 public:

  virtual unsigned int size() const=0;
  virtual EVENT_TYPE getEvent(unsigned int i) const=0;
  virtual bool Add(const EVENT_TYPE & evt)=0;

};
}//namespace MINT
#endif
//
