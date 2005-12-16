// $Id: VeloEvent_load.cpp,v 1.4 2005-12-16 10:11:03 szumlat Exp $
// Include files 
#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in VeloEvent
//
// 08/05/2002: Marco Cattaneo
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/MCVeloFE.h"
_ImplementContainedObjectFactory(MCVeloFE)
_ImplementDataObjectFactory(MCVeloFEs)

/// ==================================================================
#include "Event/VeloDigit.h"
_ImplementContainedObjectFactory(VeloDigit)
_ImplementDataObjectFactory(VeloDigits)

/// ===================================================================
#include "Event/VeloCluster.h"
_ImplementContainedObjectFactory(VeloCluster)
_ImplementDataObjectFactory(VeloClusters)

/// ====================================================================

void VeloEvent_load() {

  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( MCVeloFE );
  DLL_DECL_OBJECTFACTORY( MCVeloFEs );

  DLL_DECL_OBJECTFACTORY(VeloDigit);
  DLL_DECL_OBJECTFACTORY(VeloDigits);
  
  DLL_DECL_OBJECTFACTORY( VeloCluster );
  DLL_DECL_OBJECTFACTORY( VeloClusters );
  
}

extern "C" void VeloEvent_loadRef()  {
  VeloEvent_load();
}
