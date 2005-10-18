// $Id: EventBase_load.cpp,v 1.2 2005-10-18 14:09:34 pkoppenb Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in EventBase
//
// 17/08/2005: P. Koppenburg
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/EventHeader.h"
_ImplementDataObjectFactory(EventHeader);

/// ====================================================================
#include "Event/ProcessHeader.h"
_ImplementDataObjectFactory(ProcessHeader);

void EventBase_load() {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY( ProcessHeader );
  DLL_DECL_OBJECTFACTORY( EventHeader );

}
extern "C" void EventBase_loadRef()  {
  EventBase_load();
}
