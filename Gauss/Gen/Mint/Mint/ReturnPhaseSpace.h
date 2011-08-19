#ifndef RETURN_PHASESPACE_HH
#define RETURN_PHASESPACE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/DalitzEventAccess.h"
#include "Mint/IReturnReal.h"
#include "Mint/IDalitzEventList.h"

class ReturnPhaseSpace : public DalitzEventAccess
, virtual public MINT::IGetRealEvent<IDalitzEvent>{
 public:
  ReturnPhaseSpace(IDalitzEventAccess* evts);
  ReturnPhaseSpace(IDalitzEventList* evts);
  ReturnPhaseSpace(const ReturnPhaseSpace& other);

  double RealVal();
};

#endif
//
