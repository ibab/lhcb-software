// $Id: LinkerEvent_load.cpp,v 1.2 2004-01-15 14:24:49 ocallot Exp $
// Include files 


#include "GaudiKernel/ContainerFactoryDefs.h"

#include "Event/LinksByKey.h"
_ImplementDataObjectFactory( LinksByKey )

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

void LinkerEvent_load() {
  DLL_DECL_OBJECTFACTORY( LinksByKey  );
}

extern "C" void LinkerEvent_loadRef() 
{
  LinkerEvent_load();
}
