// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"


//From TfData
#include "TfKernel/STHit.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeTTSector.h"

// local
#include "VeloTTHitManager.h"
#include "VeloTTHit.h"

using namespace Tf;

DECLARE_TOOL_FACTORY( VeloTTHitManager );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloTTHitManager::VeloTTHitManager( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : TTStationHitManager<VeloTTHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
VeloTTHitManager::~VeloTTHitManager() { }

