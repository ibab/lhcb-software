// $Id: DAQEvent_load.cpp,v 1.2 2003-09-23 08:45:49 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

/// ====================================================================
#include "Event/HLTBuffer.h"
#include "Event/HLTEvent.h"

_ImplementDataObjectFactory( HLTBuffer )
_ImplementDataObjectFactory( HLTEvent )

/// ====================================================================
#include "Event/L1Buffer.h"
#include "Event/L1Event.h"

_ImplementDataObjectFactory( L1Buffer )
_ImplementDataObjectFactory( L1Event )


void DAQEvent_load() {

  DLL_DECL_OBJECTFACTORY( HLTBuffer );
  DLL_DECL_OBJECTFACTORY( HLTEvent );
  
  DLL_DECL_OBJECTFACTORY( L1Buffer );
  DLL_DECL_OBJECTFACTORY( L1Event );
}

extern "C" void DAQEvent_loadRef() 
{
  DAQEvent_load();
}
