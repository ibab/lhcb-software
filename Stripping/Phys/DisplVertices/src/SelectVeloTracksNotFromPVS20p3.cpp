// Include files
#include <boost/foreach.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb
#include "Event/RecVertex.h"

#include "LoKi/TrackCuts.h"
#include "LoKi/Operators.h"
#include "KalmanFilter/FastVertex.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// local
#include "SelectVeloTracksNotFromPVS20p3.h"

//-----------------------------------------------------------------------------
// Implementation file for class: SelectVeloTracksNotFromPV, Stripping20p3 version
//
// @author Neal Gauvin
// @author Victor Coco
// @author Pieter David
// @date   2012-03-27
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SelectVeloTracksNotFromPVS20p3 )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelectVeloTracksNotFromPVS20p3::SelectVeloTracksNotFromPVS20p3( const std::string& name ,
                                                      ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name , pSvcLocator )
  , m_DOCABL( LoKi::Constant<const LHCb::Track*,double>(-1.) )
  , m_uniqueSegmentSelector(0)
{
  declareProperty( "Inputs"
                   , m_TracksLocations
                   , "Input track locations" ) ;

  declareProperty( "Output"
                   , m_WithIPTrackLocation = "Hlt2/VeloWithIP/Track"
                   , "Output track location" );

  declareProperty( "PVLocation"
                   , m_PVLocation = LHCb::RecVertexLocation::Primary
                   , "Primary Vertex location" );

  declareProperty( "RemoveBackwardTracks"
                   , m_removeBackwardTracks = true
                   , "Remove backward tracks" );

  declareProperty( "MinIP"
                   , m_ipcut = 0.3*Gaudi::Units::mm
                   , "Minimum IP cut value (negative for no cut)" );

  declareProperty( "MinIPChi2"
                   , m_ipchi2cut = -1.0
                   , "Minimum IPChi2 cut value (negative for no cut)" );

  declareProperty( "MinDOCABeamLine"
                   , m_docablcut = -2.0
                   , "Minimum DOCA to the beam line (negative for no cut)" );

  declareProperty( "RemoveVeloClones"
                   , m_removeVeloClones = true
                   , "Use TrackUniqueSegmentSelector" );

  declareProperty( "RemovePVTracks"
                   , m_removePVTracks = true
                   , "Remove tracks that are in a PV (if true, IP and IPCHI2 cuts are ignored)" );

  declareProperty( "MaxNumOutputTracks"
                   , m_maxNumOutputTracks = boost::numeric::bounds<unsigned int>::highest()
                   , "Maximal number of output tracks" );

  declareProperty( "MinNumTracks"
                   , m_minNumTracks = 4
                   , "Minimal number of output tracks required to accept the event" );

  declareProperty( "UniqueVeloSegmentSelector"
                   , m_uniqueSegmentSelectorName = "TrackUniqueSegmentSelectorS20p3"
                   , "Unique Velo segment selector tool handle" );
}
//=============================================================================
// Destructor
//=============================================================================
SelectVeloTracksNotFromPVS20p3::~SelectVeloTracksNotFromPVS20p3() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode SelectVeloTracksNotFromPVS20p3::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }
  m_uniqueSegmentSelector = tool<ITrackUniqueSegmentSelector>( m_uniqueSegmentSelectorName, this );
  if ( m_uniqueSegmentSelector == 0 ) { return Error("Could not retrieve unique Velo segment selector", sc); }

  m_inputTracks.reserve(500);
  m_tracksWithUniqueVelo.reserve(500);

  m_debug   = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);

  if (m_debug) { debug() << "==> Initialize the VeloWithIP algorithm" << endmsg; }

  m_DOCABL = LoKi::Cuts::Tr_FASTDOCATOBEAMLINE(10.);

  return sc;
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
StatusCode SelectVeloTracksNotFromPVS20p3::execute()
{
  if (m_debug) { debug() << "==> Execute" << endmsg; }

  setFilterPassed(false);

  if ( ! exist<LHCb::RecVertex::Range>(m_PVLocation) )
  {
    return Warning("No RecVertex container found at " + m_PVLocation, StatusCode::SUCCESS, 1 );
  }
  const LHCb::RecVertex::Range primaryVertices = get<LHCb::RecVertex::Range>( m_PVLocation );
  if (m_verbose) { verbose() << "Number of primary vertices: " 
                             << primaryVertices.size() << endmsg; }

  m_inputTracks.clear();
  LoKi::TrackTypes::TrCut sel = LoKi::Cuts::TrHASVELO;
  if ( m_removeBackwardTracks ) {
    sel = sel && ( ! LoKi::Cuts::TrBACKWARD );
  }
  BOOST_FOREACH( const std::string& iLocation, m_TracksLocations ) {
    if ( ! exist<LHCb::Track::Range>(iLocation) ) {
      Warning("No tracks found at "+iLocation, StatusCode::SUCCESS);
    } else {
      const LHCb::Track::Range trackContainer = get<LHCb::Track::Range>(iLocation);
      if (m_verbose) { verbose() << "Number of tracks at " << iLocation << " : " << trackContainer.size() << endmsg; }
      std::copy_if( trackContainer.begin(), trackContainer.end(), std::back_inserter(m_inputTracks), sel );
    }
  }
  counter("# input tracks with Velo segment") += m_inputTracks.size();

  LHCb::Track::ConstVector& preselTracks = m_inputTracks;
  if ( m_removeVeloClones ) {
    m_tracksWithUniqueVelo.clear();
    m_uniqueSegmentSelector->select( LHCb::Track::Range(m_inputTracks.begin(), m_inputTracks.end()), m_tracksWithUniqueVelo );
    counter("# unique velo segments") += m_tracksWithUniqueVelo.size();
    preselTracks = m_tracksWithUniqueVelo;
  }

  // apply IP cuts
  LHCb::Tracks* selectedTracks = new LHCb::Tracks();
  put( selectedTracks, m_WithIPTrackLocation );
  if ( m_removePVTracks ) { // cfr. LoKi::Particles::HasTracks
    // collect PV tracks
    m_allPVTracks.clear();
    BOOST_FOREACH( const LHCb::RecVertex* pv, primaryVertices ) {
      const SmartRefVector<LHCb::Track>& pvTracks = pv->tracks();
      m_allPVTracks.addObjects(pvTracks.begin(), pvTracks.end());
    }
    // select tracks if they are not in a PV
    BOOST_FOREACH( const LHCb::Track* iTr, preselTracks ) {
      if ( ! std::binary_search( m_allPVTracks.begin(), m_allPVTracks.end(), iTr ) ) {
        selectedTracks->insert(iTr->clone());
      }
    }
    m_allPVTracks.clear();
  } else {
    BOOST_FOREACH( const LHCb::Track* iTr, preselTracks ) {
      if ( ( ( ( m_ipcut < 0. ) && ( m_ipchi2cut < 0. ) ) || ( std::find_if( primaryVertices.begin(), primaryVertices.end(), CutIPAndChi2(iTr, m_ipcut, m_ipchi2cut) ) == primaryVertices.end() ) ) && ( m_docablcut < m_DOCABL(iTr) ) ) {
        selectedTracks->insert(iTr->clone());
      }
    }
  }
  counter("# accepted tracks (all events)") += selectedTracks->size();

  if ( selectedTracks->size() > m_maxNumOutputTracks )
  {
    ++counter("# rejected due to a too large number of output tracks");
  }
  if ( selectedTracks->size() >= m_minNumTracks )
  {
    counter("# accepted tracks (accepted events)") += selectedTracks->size();
    setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelectVeloTracksNotFromPVS20p3::finalize()
{
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}
