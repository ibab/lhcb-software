
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "BeamGasTrigCheckL0AndTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigCheckL0AndTracks
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigCheckL0AndTracks );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigCheckL0AndTracks::BeamGasTrigCheckL0AndTracks(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );  
  declareProperty("PVRTTracksLocation", m_PVRTTracksLocation = "Hlt/Track/RZVelo" );
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigCheckL0AndTracks::~BeamGasTrigCheckL0AndTracks() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigCheckL0AndTracks::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigCheckL0AndTracks::execute()
{
  debug() << "==> Execute" << endmsg;
  
  // Get The L0 Decision and reject the event if it is negative. Is this possible?
  if( get<LHCb::L0DUReport>(m_l0Location)->decision() == false )
  {
    info() << "L0 decision is FALSE ==> rejecting the event!!!" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS; 
  }
       
  // If the PatVeloRTracking output container is empty ==>
  // reject the event. If it doesn't exist ==> exception (from Gaudi?)
  if( get<LHCb::Tracks>(m_PVRTTracksLocation)->empty() )
  {   
    info() << "PatVeloRTracking output container is empty!" << endmsg;
    setFilterPassed(false); 
    return StatusCode::SUCCESS;
  }

  setFilterPassed(true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BeamGasTrigCheckL0AndTracks::finalize()
{
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


