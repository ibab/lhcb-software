// $Id: DAQEvent_load.cpp,v 1.1.1.1 2003-09-10 11:41:21 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

/// ====================================================================
#include "Event/HLTBuffer.h"
#include "Event/HLTEvent.h"

///_ImplementContainedObjectFactory( HltTrack )
///_ImplementDataObjectFactory( HltTracks )
_ImplementDataObjectFactory( HLTBuffer )
_ImplementDataObjectFactory( HLTEvent )

void DAQEvent_load() {
  //DLL_DECL_OBJECTFACTORY( HltTrack  );
  //DLL_DECL_OBJECTFACTORY( HltTracks );

  DLL_DECL_OBJECTFACTORY( HLTBuffer );
  DLL_DECL_OBJECTFACTORY( HLTEvent );
  
}

extern "C" void DAQEvent_loadRef() 
{
  DAQEvent_load();
}
