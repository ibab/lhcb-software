// $Id: PhysEvent_load.cpp,v 1.4 2002-04-15 11:37:31 gcorti Exp $

// Include files
#include "Event/EventFactoryDefs.h"

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

// ===================================================================
#include "Event/AxPartCandidate.h"
_ImplementContainedObjectFactory( AxPartCandidate )
_ImplementDataObjectFactory( AxPartCandidates )

// ====================================================================
void PhysEvent_load()  {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY  ( PhysSel );

  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( Particle );
  DLL_DECL_OBJECTFACTORY( Particles );

  DLL_DECL_OBJECTFACTORY( Vertex );
  DLL_DECL_OBJECTFACTORY( Vertices );
  
  DLL_DECL_OBJECTFACTORY( AxPartCandidate );
  DLL_DECL_OBJECTFACTORY( AxPartCandidates );

}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

