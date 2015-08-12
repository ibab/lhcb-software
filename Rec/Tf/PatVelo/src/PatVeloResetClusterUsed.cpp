// Include files

// local
#include "PatVeloResetClusterUsed.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloResetClusterUsed
//
// 2008-08-26 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  DECLARE_ALGORITHM_FACTORY( PatVeloResetClusterUsed )
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

Tf::PatVeloResetClusterUsed::PatVeloResetClusterUsed( const std::string& name,
    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_rHitManager(NULL)
  , m_phiHitManager(NULL)
{
  declareProperty( "RHitManagerName", m_rHitManagerName="PatVeloRHitManager");
  declareProperty( "PhiHitManagerName", m_phiHitManagerName="PatVeloPhiHitManager");
  declareProperty( "ResetRClusters", m_resetR = false);
  declareProperty( "ResetPhiClusters", m_resetPhi = true);
}

//=============================================================================
// Destructor
//=============================================================================

Tf::PatVeloResetClusterUsed::~PatVeloResetClusterUsed() {}

//=============================================================================
// Initialization
//=============================================================================

StatusCode Tf::PatVeloResetClusterUsed::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  m_isDebug   = msgLevel( MSG::DEBUG   );
 
  if( m_isDebug ) debug() << "==> Initialize" << endmsg;

  m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
  m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode Tf::PatVeloResetClusterUsed::execute() {

  if( m_isDebug ) debug() << "==> Execute" << endmsg;
  
  if(m_resetR) m_rHitManager->resetUsedFlagOfHits();
  if(m_resetPhi) m_phiHitManager->resetUsedFlagOfHits();
 
  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVeloResetClusterUsed::finalize() {
  if( m_isDebug ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}


