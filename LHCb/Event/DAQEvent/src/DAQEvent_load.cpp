// $Id: DAQEvent_load.cpp,v 1.4 2003-11-24 16:06:06 cattanem Exp $
// Include files 

#include "Event/EventFactoryDefs.h"

/// ====================================================================
#include "Event/RawBuffer.h"
#include "Event/RawEvent.h"

_ImplementDataObjectFactory( RawBuffer )
_ImplementDataObjectFactory( RawEvent )

/// ====================================================================
#include "Event/L1Buffer.h"
#include "Event/L1Event.h"

_ImplementDataObjectFactory( L1Buffer )
_ImplementDataObjectFactory( L1Event )


void DAQEvent_load() {

  DLL_DECL_OBJECTFACTORY( RawBuffer );
  DLL_DECL_OBJECTFACTORY( RawEvent );
  
  DLL_DECL_OBJECTFACTORY( L1Buffer );
  DLL_DECL_OBJECTFACTORY( L1Event );
}

extern "C" void DAQEvent_loadRef() 
{
  DAQEvent_load();
}
