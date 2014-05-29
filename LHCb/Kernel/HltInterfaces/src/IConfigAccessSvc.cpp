#include "Kernel/IConfigAccessSvc.h"

/// Return the interface ID
const InterfaceID& IConfigAccessSvc::interfaceID() { 
      static const InterfaceID id( "IConfigAccessSvc", 2, 0 );
      return id; 
}

IConfigAccessSvc::~IConfigAccessSvc()
{}

