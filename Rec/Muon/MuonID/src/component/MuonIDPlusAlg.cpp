#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonIDPlusAlg.h"
#include "MuonID/IMuonIDTool.h"
#include "Event/MuonPID.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonIDPlusAlg
//
// novel MuonID algorithm based on the MuonIDPlusTool
//
// 2015-11-18 : Giacomo Graziani
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonIDPlusAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonIDPlusAlg::MuonIDPlusAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // input track location
  declareProperty("TrackLocation",
                  m_BestTrackLocation = LHCb::TrackLocation::Default);
  // output MuonPID location
  declareProperty("MuonIDLocation",
                  m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);

  // output MuonTracks location
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);

}
//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonIDPlusAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_muonIDtool = tool<IMuonIDTool>("MuonIDPlusTool", this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonIDPlusAlg::execute() {

  debug() << "==> Execute" << endmsg;

  // initialize MuonPID container
  auto muPids = getOrCreate<LHCb::MuonPIDs,LHCb::MuonPIDs>(m_MuonPIDsPath);
  if ( !muPids->empty() )
  {
    muPids->clear();
    Warning( "MuonPIDs already exist at '" + m_MuonPIDsPath + "' -> Will Replace",
             StatusCode::SUCCESS,1 ).ignore();
  }
  // initialize muon tracks container
  auto muTracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_MuonTracksPath);
  if ( !muTracks->empty() )
  {
    muTracks->clear();
    Warning( "Muon Tracks already exist at '" + m_MuonTracksPath + "' -> Will Replace",
             StatusCode::SUCCESS,1 ).ignore();
  }

  const LHCb::Tracks* bestTracks = get<LHCb::Tracks>( m_BestTrackLocation );
  for (const auto& t : *bestTracks) {
    // IMuonIDTool interface is not very informative about ownership...
    // (esp. about the muTrack below!)
    std::unique_ptr<LHCb::MuonPID> pid{ m_muonIDtool->getMuonID(t) };
    if(!pid) continue;
    std::unique_ptr<LHCb::Track> muTrack{const_cast<LHCb::Track*>(pid->muonTrack()) };

    if(pid->InAcceptance()) { // store it
       muPids->insert(pid.release());
       if(muTrack) muTracks->insert( muTrack.release() );
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
