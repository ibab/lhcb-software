// $Id: L0Event_load.cpp,v 1.5 2004-12-10 14:48:23 cattanem Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in Event
//
// 12/03/2002: Gloria Corti
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/L0DUReport.h"
_ImplementDataObjectFactory(L0DUReport);

/// ====================================================================
#include "Event/L0PuVeto.h"
_ImplementDataObjectFactory(L0PuVeto);

/// ====================================================================
#include "Event/L0CaloAdc.h"
_ImplementContainedObjectFactory(L0CaloAdc)
_ImplementDataObjectFactory(L0CaloAdcs)

/// ====================================================================
#include "Event/L0CaloCandidate.h"
_ImplementContainedObjectFactory(L0CaloCandidate)
_ImplementDataObjectFactory(L0CaloCandidates)

/// ===================================================================
#include "Event/L0MuonCandidate.h"
_ImplementContainedObjectFactory(L0MuonCandidate)
_ImplementDataObjectFactory(L0MuonCandidates)

/// ===================================================================
#include "Event/L0PrsSpdHit.h"
_ImplementContainedObjectFactory(L0PrsSpdHit)

/// ====================================================================

void L0Event_load() {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY( L0DUReport );
  DLL_DECL_OBJECTFACTORY( L0PuVeto );
  
  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( L0CaloAdc );
  DLL_DECL_OBJECTFACTORY( L0CaloAdcs );

  DLL_DECL_OBJECTFACTORY( L0CaloCandidate );
  DLL_DECL_OBJECTFACTORY( L0CaloCandidates );
  
  DLL_DECL_OBJECTFACTORY( L0MuonCandidate );
  DLL_DECL_OBJECTFACTORY( L0MuonCandidates );
  
  DLL_DECL_OBJECTFACTORY( L0PrsSpdHit );
}

extern "C" void L0Event_loadRef()  {
  L0Event_load();
}
