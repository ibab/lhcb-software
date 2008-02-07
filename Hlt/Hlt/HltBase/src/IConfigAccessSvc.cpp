#include "HltBase/IConfigAccessSvc.h"

/// Return the interface ID
const InterfaceID& IConfigAccessSvc::interfaceID() { 
      static const InterfaceID id( "IConfigAccessSvc", 1, 0 );
      return id; 
}

IConfigAccessSvc::~IConfigAccessSvc()
{}

