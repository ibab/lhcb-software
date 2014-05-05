#ifndef IUNWEIGHTED_EVENT_GENERATOR_HH
#define IUNWEIGHTED_EVENT_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/counted_ptr.h"
#include "Mint/IEventGenerator.h"
//#include "TRandom.h"

class TRandom;

namespace MINT{
  // (maybe this belongs in the fitter, not in mint)

template<typename RETURN_TYPE>
class IUnweightedEventGenerator : public virtual IEventGenerator<RETURN_TYPE>{
 public:
  virtual counted_ptr<RETURN_TYPE> newEvent()=0; 
  virtual counted_ptr<RETURN_TYPE> newUnweightedEvent()=0; 

  virtual bool ensureFreshEvents()=0;
  virtual bool setRnd(TRandom* rnd)=0;

  virtual ~IUnweightedEventGenerator(){}
};

}//namespace MINT
#endif
//
