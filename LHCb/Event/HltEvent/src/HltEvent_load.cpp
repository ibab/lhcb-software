// $Id: HltEvent_load.cpp,v 1.5 2006-01-18 16:35:14 hernando Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

/// ====================================================================
#include "Event/HltSummary.h"

_ImplementDataObjectFactory( HltSummary )

void HltEvent_load() {
  DLL_DECL_OBJECTFACTORY( HltSummary );
}

extern "C" void HltEvent_loadRef() 
{
  HltEvent_load();
}
