// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "PatKernel/PatUTStationHitManager.h"


DECLARE_TOOL_FACTORY( PatUTStationHitManager )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatUTStationHitManager::PatUTStationHitManager( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : Tf::UTStationHitManager<PatUTHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
PatUTStationHitManager::~PatUTStationHitManager() { }
