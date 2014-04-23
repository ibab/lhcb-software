#ifndef IEVENT_GENERATOR_HH
#define IEVENT_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/counted_ptr.h"

namespace MINT{

template<typename RETURN_TYPE>
class IEventGenerator{
 public:
  virtual counted_ptr<RETURN_TYPE> newEvent()=0; 
  virtual bool exhausted() const=0;
  virtual bool ensureFreshEvents()=0; // will normally randomise the seed
  virtual ~IEventGenerator(){}
};

}//namespace MINT
#endif
//
