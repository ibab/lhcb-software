// $Id: PhysEvent_load.cpp,v 1.5 2002-05-23 23:03:19 gcorti Exp $

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
#include "Event/FlavourTag.h"
_ImplementContainedObjectFactory( FlavourTag )
_ImplementDataObjectFactory( FlavourTags )

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
  
  DLL_DECL_OBJECTFACTORY( FlavourTag );
  DLL_DECL_OBJECTFACTORY( FlavourTags );
  
  DLL_DECL_OBJECTFACTORY( AxPartCandidate );
  DLL_DECL_OBJECTFACTORY( AxPartCandidates );

}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

