#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TfKernel/DefaultVeloPhiHitManager.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DefaultVeloPhiHitManager
//
// 2007-08-07 : Kurt Rinnert <kurt.rinnert@cern.ch>
//-----------------------------------------------------------------------------
namespace Tf {

DECLARE_TOOL_FACTORY( DefaultVeloPhiHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DefaultVeloPhiHitManager::DefaultVeloPhiHitManager(const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>(type, name, parent)
{
  declareInterface<DefaultVeloPhiHitManager>(this);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DefaultVeloPhiHitManager::initialize()
{
  StatusCode sc = DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode DefaultVeloPhiHitManager::finalize()
{
  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;


  return DefaultVeloHitManager<DeVeloPhiType,VeloPhiHit,2>::finalize();  // must be called after all other actions
}

} // namespace Tf
