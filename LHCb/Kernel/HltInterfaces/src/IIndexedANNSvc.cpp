#include "Kernel/IIndexedANNSvc.h"
/// Return the interface ID
const InterfaceID& IIndexedANNSvc::interfaceID() { 
      static const InterfaceID id( "IIndexedANNSvc", 1, 0 );
      return id; 
}
IIndexedANNSvc::~IIndexedANNSvc() 
{ }
