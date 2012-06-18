// Include files
#include <boost/foreach.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb
#include "Event/RecVertex.h"
#include "Event/ProcStatus.h"

#include "LoKi/Constants.h"
#include "LoKi/TrackCuts.h"
#include "KalmanFilter/FastVertex.h"

// local
#include "SelectVeloTracksNotFromPV.h"

using namespace LoKi::Cuts;

//-----------------------------------------------------------------------------
// Implementation file for class: SelectVeloTracksNotFromPV
//
// @author Neal Gauvin
// @author Victor Coco
// @author Pieter David
// @date   2012-03-27
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SelectVeloTracksNotFromPV );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelectVeloTracksNotFromPV::SelectVeloTracksNotFromPV( const std::string& name , ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name , pSvcLocator )
{
  declareProperty( "Inputs"
                 , m_TracksLocations
                 , "Input track locations" ) ;
  m_TracksLocations.push_back("Hlt2/Track/Velo");

  declareProperty( "Output"
                 , m_WithIPTrackLocation = "Hlt2/VeloWithIP/Track"
                 , "Output track location" );

  declareProperty( "PVLocation"
                 , m_PVLocation = "Hlt/Vertex/PV3D"
                 , "Primary Vertex location" );

  declareProperty( "RejectSplashEvents"
                 , m_rejectSplashEvents = true
                 , "Reject all FastVelo beam splash events, cfr. FastVeloTracking::beamSplashSpaceMerge" );

  declareProperty( "RemoveBackwardTracks"
                 , m_removeBackwardTracks = true
                 , "Remove backward tracks" );

  declareProperty( "MinIP"
                 , m_ipcut = 0.3*Gaudi::Units::mm
                 , "Minimum IP cut value (negative for no cut)" );

  declareProperty( "MinIPChi2"
                 , m_ipchi2cut = -1.0
                 , "Minimum IPChi2 cut value (negative for no cut)" );

  declareProperty( "MaxNumInputTracks"
                 , m_maxNumInputTracks = -1
                 , "Maximal number of input tracks" );

  declareProperty( "MaxNumOutputTracks"
                 , m_maxNumOutputTracks = -1
                 , "Maximal number of output tracks" );

  declareProperty( "MinNumTracks"
                 , m_minNumTracks = 4
                 , "Minimal number of output tracks required to accept the event" );

}
//=============================================================================
// Destructor
//=============================================================================
SelectVeloTracksNotFromPV::~SelectVeloTracksNotFromPV() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode SelectVeloTracksNotFromPV::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_debug = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);

  if (m_debug) { debug() << "==> Initialize the VeloWithIP algorithm" << endmsg; }

  return StatusCode::SUCCESS;

}

namespace {
  // Functor version of LoKi::FastVertex::checkDistance
  class CutIPAndChi2
  {
  public:
    CutIPAndChi2( const LHCb::Track* track, const double& maxIP, const double& maxIPChi2, bool iterate=false )
    : m_track(track)
    , m_maxIP(maxIP)
    , m_maxIPChi2(maxIPChi2)
    , m_iterate(iterate)
    {}
    bool operator() ( const LHCb::VertexBase* vx )
    {
      return LoKi::FastVertex::checkDistance( m_track, vx, m_maxIP, m_maxIPChi2, m_iterate );
    }
  private:
    const LHCb::Track* m_track;
    const double& m_maxIP;
    const double& m_maxIPChi2;
    bool m_iterate;
  };
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SelectVeloTracksNotFromPV::execute()
{
  if (m_debug) { debug() << "==> Execute" << endmsg; }

  setFilterPassed(false);

  // If needed, reject splash events immedeately
  if ( m_rejectSplashEvents ) {
    if ( exist<LHCb::ProcStatus>( LHCb::ProcStatusLocation::Default ) ) {
      LHCb::ProcStatus* procStat = get<LHCb::ProcStatus>( LHCb::ProcStatusLocation::Default );
      for( LHCb::ProcStatus::AlgStatusVector::const_iterator iAlg = procStat->algs().begin(); iAlg != procStat->algs().end(); ++iAlg ) {
        if ( m_debug ) { debug() << "Found ProcStatus entry: " << (*iAlg).first << " (" << (*iAlg).second << ")" << endmsg; }
        if ( (*iAlg).first.compare("OK:VELO:BeamSplashFastVelo:FastVeloTracking") == 0 ) {
          if ( m_debug ) { debug() << "==> Rejecting event" << endmsg; }
          ++counter("#rejected splash");
          return StatusCode::SUCCESS;
        }
      }
    }
  }

  if ( ! exist<LHCb::RecVertex::Range>(m_PVLocation) ) {
    return Warning("No RecVertex container found at " + m_PVLocation, StatusCode::SUCCESS, 1 );
  }
  LHCb::RecVertex::Range primaryVertices = get<LHCb::RecVertex::Range>( m_PVLocation );
  if (m_verbose) { verbose() << "Number of primary vertices: " << primaryVertices.size() << endmsg; }

  LHCb::Tracks* iptracks = new LHCb::Tracks();
  put( iptracks, m_WithIPTrackLocation );

  int nInput = 0;
  BOOST_FOREACH( std::string iLocation, m_TracksLocations ) {
    if ( ! exist<LHCb::Tracks>(iLocation) ) {
      return Warning( "No tracks found at " + iLocation, StatusCode::SUCCESS, 1 );
    }
    LHCb::Track::Range trackContainer = get<LHCb::Track::Range>(iLocation);
    if (m_verbose) { verbose() << "Number of tracks: " << trackContainer.size() << endmsg; }
    nInput += trackContainer.size();
    if ( ( m_maxNumInputTracks > 0 ) && ( nInput > m_maxNumInputTracks ) ) {
      ++counter("# rejected due to a too large number of input tracks");
      return StatusCode::SUCCESS;
    }
    counter("# " + iLocation) += trackContainer.size();

    // add all forward tracks passing the MinIP requirement
    BOOST_FOREACH( const LHCb::Track* iTr, trackContainer ) {
      if (m_verbose) { verbose() << "Track " << iTr->key() << " " << ( ( TrBACKWARD(iTr) ) ? "backward" : "forward" ) << endmsg; }

      if ( ( ! ( m_removeBackwardTracks && iTr->checkFlag(LHCb::Track::Backward) ) )
        && ( std::find_if( primaryVertices.begin(), primaryVertices.end(), CutIPAndChi2(iTr, m_ipcut, m_ipchi2cut) ) == primaryVertices.end() ) )
      {
        if (m_verbose) { verbose() << "inserting track " << endmsg; }
        iptracks->add(iTr->clone());
      }
    }
  }
  counter("# input tracks") += nInput;

  if (m_debug) { debug() << "Accepted " << iptracks->size() << " tracks => " << ( ( iptracks->size() >= m_minNumTracks ) ? "Accepting" : "Rejecting" ) << " event"  << endmsg; }
  counter("# accepted tracks (all events)") += iptracks->size();

  if ( ( m_maxNumOutputTracks > 0 ) && ( iptracks->size() > m_maxNumOutputTracks ) ) {
    ++counter("# rejected due to a too large number of output tracks");
    return StatusCode::SUCCESS;
  }
  if ( iptracks->size() >= m_minNumTracks ) {
    counter("# accepted tracks (accepted events)") += iptracks->size();
    setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelectVeloTracksNotFromPV::finalize()
{
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}
