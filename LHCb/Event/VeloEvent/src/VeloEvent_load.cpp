// $Id: VeloEvent_load.cpp,v 1.2 2003-12-18 15:33:09 cattanem Exp $
// Include files 
#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in VeloEvent
//
// 08/05/2002: Marco Cattaneo
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/MCVeloHit.h"
_ImplementContainedObjectFactory(MCVeloHit)
_ImplementDataObjectFactory(MCVeloHits)

/// ====================================================================
#include "Event/MCVeloFE.h"
_ImplementContainedObjectFactory(MCVeloFE)
_ImplementDataObjectFactory(MCVeloFEs)

/// ===================================================================
#include "Event/VeloFullDigit.h"
_ImplementContainedObjectFactory(VeloFullDigit)
_ImplementDataObjectFactory(VeloFullDigits)

/// ===================================================================
#include "Event/VeloCluster.h"
_ImplementContainedObjectFactory(VeloCluster)
_ImplementDataObjectFactory(VeloClusters)

/// ====================================================================

void VeloEvent_load() {

  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( MCVeloHit );
  DLL_DECL_OBJECTFACTORY( MCVeloHits );  

  DLL_DECL_OBJECTFACTORY( MCVeloFE );
  DLL_DECL_OBJECTFACTORY( MCVeloFEs );
  
  DLL_DECL_OBJECTFACTORY( VeloFullDigit );
  DLL_DECL_OBJECTFACTORY( VeloFullDigits );
  
  DLL_DECL_OBJECTFACTORY( VeloCluster );
  DLL_DECL_OBJECTFACTORY( VeloClusters );
}

extern "C" void VeloEvent_loadRef()  {
  VeloEvent_load();
}
