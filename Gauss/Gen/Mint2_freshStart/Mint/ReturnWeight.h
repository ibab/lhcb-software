#ifndef RETURN_WEIGHT_HH
#define RETURN_WEIGHT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEvent.h"

// possible that this is not really
// needed anymore after moving to
// vectorised MINT - leave it for now,
// but likely to disappear in the future.

class ReturnWeight 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public MINT::IReturnRealForEvent<DalitzEvent>{
 public:
  ReturnWeight(){}
  ReturnWeight(const ReturnWeight& ){}

  double RealVal(IDalitzEvent& evt){
    return evt.getWeight();
  }
  double RealVal(DalitzEvent& evt){
    return evt.getWeight();
  }
};

#endif
//
