#include "GaudiKernel/ContainerFactoryDefs.h"

#include "Event/HepMCEvent.h"
_ImplementContainedObjectFactory( HepMCEvent  ) ;
_ImplementDataObjectFactory(      HepMCEvents ) ;

#include "Event/GenHeader.h"
_ImplementDataObjectFactory(      GenHeader ) ;

#include "Event/GenCollision.h"
_ImplementContainedObjectFactory( GenCollision ) ;
_ImplementDataObjectFactory(      GenCollisions ) ;

void GenEvent_load() {  
    DLL_DECL_OBJECTFACTORY( HepMCEvent );
    DLL_DECL_OBJECTFACTORY( HepMCEvents ); 
    DLL_DECL_OBJECTFACTORY( GenHeader );
    DLL_DECL_OBJECTFACTORY( GenCollision );
    DLL_DECL_OBJECTFACTORY( GenCollisions );
}

extern "C" void GenEvent_loadRef()  {
    GenEvent_load();
}
