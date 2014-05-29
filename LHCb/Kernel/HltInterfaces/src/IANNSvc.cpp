#include "Kernel/IANNSvc.h"

/// Return the interface ID
const InterfaceID& IANNSvc::interfaceID() { 
      static const InterfaceID id( "IANNSvc", 2, 0 );
      return id; 
}

IANNSvc::~IANNSvc()
{}

