#include "Event/EventFactoryDefs.h"

#include "Event/HepMCEvent.h"
_ImplementContainedObjectFactory( HepMCEvent  ) ;
_ImplementDataObjectFactory(      HepMCEvents ) ;

void Event_load() {  
  DLL_DECL_OBJECTFACTORY( HepMCEvent );
  DLL_DECL_OBJECTFACTORY( HepMCEvents ); 
}

extern "C" void Event_loadRef()  {
  Event_load();
}
