#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TfKernel/DefaultVeloRHitManager.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DefaultVeloRHitManager
//
// 2007-07-04 : Kurt Rinnert <kurt.rinnert@cern.ch>
//-----------------------------------------------------------------------------
namespace Tf {

DECLARE_TOOL_FACTORY( DefaultVeloRHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DefaultVeloRHitManager::DefaultVeloRHitManager(const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : DefaultVeloHitManager<DeVeloRType,VeloRHit,4>(type, name, parent)
{
  declareInterface<DefaultVeloRHitManager>(this);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DefaultVeloRHitManager::initialize()
{
  StatusCode sc = DefaultVeloHitManager<DeVeloRType,VeloRHit,4>::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode DefaultVeloRHitManager::finalize()
{
  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;


  return DefaultVeloHitManager<DeVeloRType,VeloRHit,4>::finalize();  // must be called after all other actions
}

} // namespace Tf
