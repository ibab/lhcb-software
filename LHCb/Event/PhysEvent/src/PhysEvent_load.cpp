// $Id: PhysEvent_load.cpp,v 1.1.1.1 2001-07-09 09:23:58 gcorti Exp $

// Include files
#include "LHCbEvent/EventFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in PhysEvent
//
// 28/05/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// ====================================================================
// Object factory implementation for objects of class PhysEvent
// ====================================================================
#include "PhysEvent/PhysEvent.h"
_ImplementDataObjectFactory(PhysEvent);
_ImplementDataObjectDictionaryFactory(PhysEvent)


// ====================================================================
// Object factory implementation for objects of class VtxCandidate
// ====================================================================
#include "PhysEvent/VtxCandidate.h"
_ImplementContainedFactories(VtxCandidate);

// ====================================================================
// Object factory implementation for objects of class PhysSel
// ====================================================================
#include "PhysEvent/PhysSel.h"
_ImplementDataObjectFactory(PhysSel);
_ImplementDataObjectDictionaryFactory(PhysSel)

// ====================================================================
// Object factory implementation for objects of class PhysSelDecay
// ===================================================================
#include "PhysEvent/PhysSelDecay.h"
_ImplementDictionaryFactory (PhysSelDecay)
// ===================================================================

void PhysEvent_load()  {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY  ( PhysEvent );
  DLL_DECL_OBJECTDICT     ( PhysEvent );

  DLL_DECL_OBJECTFACTORY  ( PhysSel );
  DLL_DECL_OBJECTDICT     ( PhysSel );

  DLL_DECL_OBJECTDICT     ( PhysSelDecay );
  
  // Declaration of contained object factories
  DLL_DECL_CONTAINEDOBJECTFACTORIES ( VtxCandidate );

}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

