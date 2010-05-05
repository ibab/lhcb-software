#include "Kernel/IPropertyConfigSvc.h"

/// Return the interface ID
const InterfaceID& IPropertyConfigSvc::interfaceID() { 
      static const InterfaceID id( "IPropertyConfigSvc", 1, 0 );
      return id; 
}

IPropertyConfigSvc::~IPropertyConfigSvc()
{}

