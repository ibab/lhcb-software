

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"


//From TfKernel
#include "TfKernel/STHit.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/RecoFuncs.h"

// local
#include "PatKernel/PatTTStationHitManager.h"

using namespace Tf;

DECLARE_TOOL_FACTORY( PatTTStationHitManager );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTTStationHitManager::PatTTStationHitManager( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : Tf::TTStationHitManager<Tf::PatTTHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
PatTTStationHitManager::~PatTTStationHitManager() { };

