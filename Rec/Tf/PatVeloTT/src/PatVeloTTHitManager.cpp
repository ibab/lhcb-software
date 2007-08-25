// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

//From TfData
#include "TfKernel/STHit.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeTTSector.h"

// local
#include "PatVeloTTHitManager.h"
#include "PatVeloTTHit.h"

using namespace Tf;

DECLARE_TOOL_FACTORY( PatVeloTTHitManager );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloTTHitManager::PatVeloTTHitManager( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : TTStationHitManager<PatVeloTTHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
PatVeloTTHitManager::~PatVeloTTHitManager() { }
