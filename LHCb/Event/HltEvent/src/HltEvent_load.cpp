// $Id: HltEvent_load.cpp,v 1.1.1.1 2003-04-10 08:07:22 ocallot Exp $
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
