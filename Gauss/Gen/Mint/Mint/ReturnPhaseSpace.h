#ifndef RETURN_PHASESPACE_HH
#define RETURN_PHASESPACE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzEvent.h"

class ReturnPhaseSpace : virtual public MINT::IReturnRealForEvent<IDalitzEvent>{
 public:
  double RealVal(IDalitzEvent& evt);
};

#endif
//
