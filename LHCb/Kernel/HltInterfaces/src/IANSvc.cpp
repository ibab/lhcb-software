#include "Kernel/IANSvc.h"

/// Return the interface ID
const InterfaceID& IANSvc::interfaceID() { 
      static const InterfaceID id( "IANSvc", 2, 0 );
      return id; 
}

IANSvc::~IANSvc()
{}

