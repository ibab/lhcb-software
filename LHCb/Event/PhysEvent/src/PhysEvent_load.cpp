// $Id: PhysEvent_load.cpp,v 1.13 2005-07-06 05:45:27 pkoppenb Exp $

// Include files
#include "GaudiKernel/ContainerFactoryDefs.h"

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

/// ===================================================================
#include "Event/PrimVertex.h"
_ImplementContainedObjectFactory( PrimVertex )
_ImplementDataObjectFactory( PrimVertices )

/// ====================================================================
#include "Event/FlavourTag.h"
_ImplementContainedObjectFactory( FlavourTag )
_ImplementDataObjectFactory( FlavourTags )

/// ====================================================================
#include "Event/SelResult.h"
_ImplementContainedObjectFactory( SelResult );
_ImplementDataObjectFactory( SelResults );

// ===================================================================
#include "Event/ProtoParticle.h"
_ImplementContainedObjectFactory( ProtoParticle )
_ImplementDataObjectFactory( ProtoParticles )

// ===================================================================
#include "Event/V0.h"
_ImplementContainedObjectFactory( V0 )
_ImplementDataObjectFactory( V0s )

/// ===================================================================
#include "Event/EffCheckResult.h"
_ImplementContainedObjectFactory(EffCheckResult)
_ImplementDataObjectFactory(EffCheckResults)
/// ===================================================================
#include "Event/FilterCriterionResult.h"
_ImplementContainedObjectFactory(FilterCriterionResult)
_ImplementDataObjectFactory(FilterCriterionResults)

/// ===================================================================
// ====================================================================
void PhysEvent_load()  {

  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( Particle );
  DLL_DECL_OBJECTFACTORY( Particles );

  DLL_DECL_OBJECTFACTORY( Vertex );
  DLL_DECL_OBJECTFACTORY( Vertices );
  
  DLL_DECL_OBJECTFACTORY( PrimVertex );
  DLL_DECL_OBJECTFACTORY( PrimVertices );
  
  DLL_DECL_OBJECTFACTORY( FlavourTag );
  DLL_DECL_OBJECTFACTORY( FlavourTags );
  
  DLL_DECL_OBJECTFACTORY( SelResult );
  DLL_DECL_OBJECTFACTORY( SelResults );

  DLL_DECL_OBJECTFACTORY( ProtoParticle );
  DLL_DECL_OBJECTFACTORY( ProtoParticles );

  DLL_DECL_OBJECTFACTORY( V0 );
  DLL_DECL_OBJECTFACTORY( V0s );

  DLL_DECL_OBJECTFACTORY( EffCheckResult );
  DLL_DECL_OBJECTFACTORY( EffCheckResults );

  DLL_DECL_OBJECTFACTORY( FilterCriterionResult );
  DLL_DECL_OBJECTFACTORY( FilterCriterionResults);

}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

