// $Id: EventBase_load.cpp,v 1.1.1.1 2005-09-06 07:11:44 pkoppenb Exp $
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

/// ===================================================================
#include "Event/AlgUsedTime.h"
_ImplementContainedObjectFactory(AlgUsedTime)
_ImplementDataObjectFactory(AlgUsedTimes)

/// ===================================================================
#include "Event/ProcStatus.h"
_ImplementDataObjectFactory(ProcStatus)

/// ====================================================================
#include "Event/ProcessHeader.h"
_ImplementDataObjectFactory(ProcessHeader);

void EventBase_load() {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY( ProcessHeader );
  DLL_DECL_OBJECTFACTORY( EventHeader );
  DLL_DECL_OBJECTFACTORY( AlgUsedTime );
  DLL_DECL_OBJECTFACTORY( AlgUsedTimes );
  DLL_DECL_OBJECTFACTORY( ProcStatus );

}
extern "C" void EventBase_loadRef()  {
  EventBase_load();
}
