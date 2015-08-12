// local
#include "PrUTStationHitManager.h"


DECLARE_TOOL_FACTORY( PrUTStationHitManager )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrUTStationHitManager::PrUTStationHitManager( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : Tf::UTStationHitManager<PrUTHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
PrUTStationHitManager::~PrUTStationHitManager() { }
