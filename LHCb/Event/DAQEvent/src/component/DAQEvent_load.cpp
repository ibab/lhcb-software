// $Id: DAQEvent_load.cpp,v 1.1 2004-06-28 11:41:03 cattanem Exp $

// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.
/// ====================================================================

#include "Event/RawBuffer.h"
_ImplementDataObjectFactory( RawBuffer )

#include "Event/RawEvent.h"
_ImplementDataObjectFactory( RawEvent )

#include "Event/L1Buffer.h"
_ImplementDataObjectFactory( L1Buffer )

#include "Event/L1Event.h"
_ImplementDataObjectFactory( L1Event )


DECLARE_FACTORY_ENTRIES(DAQEvent) {

  DECLARE_OBJECT( RawBuffer );
  DECLARE_OBJECT( RawEvent );
  
  DECLARE_OBJECT( L1Buffer );
  DECLARE_OBJECT( L1Event );

  DECLARE_ALGORITHM( CreateL1Buffer );
  DECLARE_ALGORITHM( CreateL1Event );
  DECLARE_ALGORITHM( CreateRawBuffer );
  DECLARE_ALGORITHM( CreateRawEvent );
}
