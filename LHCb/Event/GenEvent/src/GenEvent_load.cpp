#include "GaudiKernel/ContainerFactoryDefs.h"

#include "Event/HepMCEvent.h"
_ImplementContainedObjectFactory( HepMCEvent  ) ;
_ImplementDataObjectFactory(      HepMCEvents ) ;

#include "Event/HardInfo.h"
_ImplementContainedObjectFactory( HardInfo ) ;
_ImplementDataObjectFactory(      HardInfos ) ;

#include "Event/GenMCLink.h"
_ImplementContainedObjectFactory( GenMCLink ) ;
_ImplementDataObjectFactory(      GenMCLinks ) ;

#include "Event/GenCollision.h"
_ImplementContainedObjectFactory( GenCollision ) ;
//_ImplementDataObjectFactory(      GenCollisions ) ;

void GenEvent_load() {  
    DLL_DECL_OBJECTFACTORY( HepMCEvent );
    DLL_DECL_OBJECTFACTORY( HepMCEvents ); 
    DLL_DECL_OBJECTFACTORY( HardInfo );
    DLL_DECL_OBJECTFACTORY( HardInfos ); 
    DLL_DECL_OBJECTFACTORY( GenMCLink );
    DLL_DECL_OBJECTFACTORY( GenMCLinks );
    DLL_DECL_OBJECTFACTORY( GenCollision );
    //    DLL_DECL_OBJECTFACTORY( GenCollisions );
}

extern "C" void GenEvent_loadRef()  {
    GenEvent_load();
}
