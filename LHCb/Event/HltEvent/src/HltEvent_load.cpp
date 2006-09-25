// $Id: HltEvent_load.cpp,v 1.6 2006-09-25 13:55:47 hernando Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"
#include  "Event/HltSummary.h"

using namespace LHCb;

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
