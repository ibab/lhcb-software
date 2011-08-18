#ifndef IDALITZEVENTACCESS_HH
#define IDALITZEVENTACCESS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/Mint/Events/IEventAccess.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzIntegrator/IDalitzIntegrator.h"

class IDalitzEventAccess : virtual public MINT::IEventAccess<IDalitzEvent>{
 public:
  //  virtual IDalitzIntegrator* makeIntegratorForOwner()=0;
};
#endif
//
