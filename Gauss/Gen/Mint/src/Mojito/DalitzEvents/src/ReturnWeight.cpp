// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/ReturnWeight.h"
using namespace MINT;

ReturnWeight::ReturnWeight(IDalitzEventAccess* evts)
  : IDalitzEventAccess()
  , IReturnReal()
  , DalitzEventAccess(evts)
{}

ReturnWeight::ReturnWeight(IDalitzEventList* evts)
  : IDalitzEventAccess()
  , IReturnReal()
  , DalitzEventAccess(evts)
{}

ReturnWeight::ReturnWeight(const ReturnWeight& other)
  : IBasicEventAccess<IDalitzEvent>()
    , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , DalitzEventAccess(other)
{}

double ReturnWeight::RealVal(){
  if(0 == getEvent()) return -9999;
  return getEvent()->getWeight();
}
