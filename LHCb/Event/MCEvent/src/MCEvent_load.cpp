// $Id: MCEvent_load.cpp,v 1.1.1.1 2005-09-06 07:06:43 pkoppenb Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in MCEvent
//
// 12/03/2002: Gloria Corti
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/MCParticle.h"
_ImplementContainedObjectFactory(MCParticle)
_ImplementDataObjectFactory(MCParticles)

/// ===================================================================
#include "Event/MCVertex.h"
_ImplementContainedObjectFactory(MCVertex)
_ImplementDataObjectFactory(MCVertices)

/// ===================================================================
#include "Event/MCHit.h"
_ImplementContainedObjectFactory(MCHit)
_ImplementDataObjectFactory(MCHits)

/// ===================================================================
#include "Event/MCProperty.h"
_ImplementDataObjectFactory(MCProperty)

/// ===================================================================

#include "Event/MCEffTree.h"
_ImplementContainedObjectFactory( MCEffTree )
_ImplementDataObjectFactory( MCEffTrees )
/// ====================================================================

void MCEvent_load() {
  
  DLL_DECL_OBJECTFACTORY( MCParticle );
  DLL_DECL_OBJECTFACTORY( MCParticles );
  
  DLL_DECL_OBJECTFACTORY( MCVertex );
  DLL_DECL_OBJECTFACTORY( MCVertices );
  
  DLL_DECL_OBJECTFACTORY( MCHit );
  DLL_DECL_OBJECTFACTORY( MCHits );

  DLL_DECL_OBJECTFACTORY( MCProperty );

  DLL_DECL_OBJECTFACTORY( MCEffTree );
  DLL_DECL_OBJECTFACTORY( MCEffTrees );

  DLL_DECL_OBJECTFACTORY( GenHeader );
}

extern "C" void MCEvent_loadRef()  {
  MCEvent_load();
}
