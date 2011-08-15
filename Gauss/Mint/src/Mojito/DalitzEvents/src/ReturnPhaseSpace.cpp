// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/ReturnPhaseSpace.h"
using namespace MINT;

ReturnPhaseSpace::ReturnPhaseSpace(IDalitzEventAccess* evts)
  : IDalitzEventAccess()
  , IReturnReal()
  , DalitzEventAccess(evts)
{}

ReturnPhaseSpace::ReturnPhaseSpace(IDalitzEventList* evts)
  : IDalitzEventAccess()
  , IReturnReal()
  , DalitzEventAccess(evts)
{}

ReturnPhaseSpace::ReturnPhaseSpace(const ReturnPhaseSpace& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , DalitzEventAccess(other)
{}

double ReturnPhaseSpace::RealVal(){
  if(0 == getEvent()) return -9999;
  return getEvent()->phaseSpace();
}
