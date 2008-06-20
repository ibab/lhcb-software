#include "HltBase/IHltDataSvc.h"

/// Return the interface ID
const InterfaceID& IHltDataSvc::interfaceID() { 
      static const InterfaceID id( "IHltDataSvc", 1, 0 );
      return id; 
}

IHltDataSvc::~IHltDataSvc()
{}
