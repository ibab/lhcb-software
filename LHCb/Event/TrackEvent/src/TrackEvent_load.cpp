// Include files 
#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in TrackEvent
//
// 10/02/2005: Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/TrackVertex.h"
_ImplementContainedObjectFactory( TrackVertex )
_ImplementDataObjectFactory( TrackVertices )


void TrackEvent_load() {
  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( TrackVertex );
  DLL_DECL_OBJECTFACTORY( TrackVertices );
}

extern "C" void TrackEvent_loadRef()  {
  TrackEvent_load();
}
