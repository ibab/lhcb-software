// $Id: HltEvent_load.cpp,v 1.3 2003-09-25 12:28:33 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

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
