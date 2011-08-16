#ifndef RETURN_WEIGHT_HH
#define RETURN_WEIGHT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "DalitzEventAccess.h"
#include "IReturnReal.h"
#include "IDalitzEventList.h"

class ReturnWeight 
: public DalitzEventAccess
, virtual public MINT::IGetRealEvent<IDalitzEvent>{
 public:
  ReturnWeight(IDalitzEventAccess* evts);
  ReturnWeight(IDalitzEventList* evts);
  ReturnWeight(const ReturnWeight& other);

  double RealVal();
};

#endif
//
