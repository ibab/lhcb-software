// $Id: DAQEvent_load.cpp,v 1.3 2003-11-03 07:41:38 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

/// ====================================================================
#include "Event/HltBuffer.h"
#include "Event/HltEvent.h"

_ImplementDataObjectFactory( HltBuffer )
_ImplementDataObjectFactory( HltEvent )

/// ====================================================================
#include "Event/L1Buffer.h"
#include "Event/L1Event.h"

_ImplementDataObjectFactory( L1Buffer )
_ImplementDataObjectFactory( L1Event )


void DAQEvent_load() {

  DLL_DECL_OBJECTFACTORY( HltBuffer );
  DLL_DECL_OBJECTFACTORY( HltEvent );
  
  DLL_DECL_OBJECTFACTORY( L1Buffer );
  DLL_DECL_OBJECTFACTORY( L1Event );
}

extern "C" void DAQEvent_loadRef() 
{
  DAQEvent_load();
}
