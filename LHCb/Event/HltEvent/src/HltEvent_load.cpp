// $Id: HltEvent_load.cpp,v 1.2 2003-06-03 06:17:34 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

/// ====================================================================
#include "Event/HltTrack.h"

_ImplementContainedObjectFactory( HltTrack )
_ImplementDataObjectFactory( HltTracks )

void HltEvent_load() {
  DLL_DECL_OBJECTFACTORY( HltTrack  );
  DLL_DECL_OBJECTFACTORY( HltTracks );
}

extern "C" void HltEvent_loadRef() 
{
  HltEvent_load();
}
