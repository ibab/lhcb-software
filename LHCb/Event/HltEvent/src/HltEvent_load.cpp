// $Id: HltEvent_load.cpp,v 1.4 2003-12-18 15:33:31 cattanem Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

/// ====================================================================
#include "Event/HltTrack.h"
_ImplementContainedObjectFactory( HltTrack )
_ImplementDataObjectFactory( HltTracks )

#include "Event/HltVertex.h"
_ImplementContainedObjectFactory( HltVertex )
_ImplementDataObjectFactory( HltVertices )


void HltEvent_load() {
  DLL_DECL_OBJECTFACTORY( HltTrack  );
  DLL_DECL_OBJECTFACTORY( HltTracks );
  DLL_DECL_OBJECTFACTORY( HltVertex );
  DLL_DECL_OBJECTFACTORY( HltVertices );
}

extern "C" void HltEvent_loadRef() 
{
  HltEvent_load();
}
