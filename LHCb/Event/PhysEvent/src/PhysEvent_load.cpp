// $Id: PhysEvent_load.cpp,v 1.2 2002-02-28 13:40:32 gcorti Exp $

// Include files
#include "LHCbEvent/EventFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in PhysEvent
//
// 28/05/2001 : Gloria Corti
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/Particle.h"
_ImplementContainedObjectFactory( Particle )
_ImplementDataObjectFactory( Particles )
/// ===================================================================
#include "Event/Vertex.h"
_ImplementContainedObjectFactory( Vertex )
_ImplementDataObjectFactory( Vertices )
/// ====================================================================
#include "Event/PhysSel.h"
_ImplementDataObjectFactory(PhysSel);
_ImplementDataObjectDictionaryFactory(PhysSel);
// ===================================================================
#include "Event/PhysSelDecay.h"
_ImplementDictionaryFactory (PhysSelDecay);
// ====================================================================
void PhysEvent_load()  {

  // Declaration of data object factories

  DLL_DECL_OBJECTFACTORY  ( PhysSel );
  DLL_DECL_OBJECTDICT     ( PhysSel );

  DLL_DECL_OBJECTDICT     ( PhysSelDecay );
  
  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( Particle );
  DLL_DECL_OBJECTFACTORY( Particles );
  DLL_DECL_OBJECTFACTORY( Vertex );
  DLL_DECL_OBJECTFACTORY( Vertices );
  
  

}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

