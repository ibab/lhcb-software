// $Id: DAQEvent_load.cpp,v 1.5 2003-12-18 15:33:01 cattanem Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

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
