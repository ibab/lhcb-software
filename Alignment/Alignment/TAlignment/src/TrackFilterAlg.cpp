// $Id: TrackFilterAlg.cpp,v 1.1 2007-07-20 17:26:53 janos Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"

// from BOOST
#include <boost/assign/list_of.hpp>
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "TrackFilterAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackFilterAlg
//
// 2007-07-11 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using namespace boost::lambda;
using namespace boost::assign;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackFilterAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackFilterAlg::TrackFilterAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_tracksInputContainer(""),
    m_tracksOutputContainer(""),
    m_trackType(""),
    m_trackSelectorName(""),
    m_trackSelector(NULL)
{
  /// Map strings to track types
  m_stringToTrackTypeMap = 
    map_list_of("Velo", Track::Velo)("VeloR", Track::VeloR)("Long", Track::Long)
    ("Upstream", Track::Upstream)("Downstream", Track::Downstream)("Ttrack", Track::Ttrack)("Muon", Track::Muon);
  /// And vice versa. I whish we had Boost::BiMap
  m_trackTypeToStringMap = 
    map_list_of(Track::Velo, "Velo")(Track::VeloR, "VeloR")(Track::Long, "Long")
    (Track::Upstream, "Upstream")(Track::Downstream, "Downstream")(Track::Ttrack, "Ttrack")(Track::Muon, "Muon");

  declareProperty("TracksInputContainer" , m_tracksInputContainer  = TrackLocation::Default    );
  declareProperty("TracksOutputContainer", m_tracksOutputContainer = "Alignment/FilteredTracks");
  declareProperty("TrackType"            , m_trackType             = "Long"                    );
  declareProperty("TrackSelector"        , m_trackSelectorName     = "AlignSelTool"            );
}

//=============================================================================
// Destructor
//=============================================================================
TrackFilterAlg::~TrackFilterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFilterAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set output container if not specified
  if ( m_tracksOutputContainer == "" ) return Error("==> Please specify an output location for the filtered tracks", 
                                                    StatusCode::FAILURE);
    
  /// Get track selector tool
  m_trackSelector = tool<IAlignSelTool>(m_trackSelectorName, "Selector", this);
  if (!m_trackSelector) return Error("==> Failed to retrieve track selector tool", StatusCode::FAILURE);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackFilterAlg::execute() {

  /// Get tracks 
  Tracks* tracks = get<Tracks>(m_tracksInputContainer);
 
  filterTracks(tracks);
 
  return StatusCode::SUCCESS;
}

//=============================================================================

void TrackFilterAlg::filterTrack(LHCb::Track* track, LHCb::Tracks* outputContainer) {
  if ((track->checkType(m_stringToTrackTypeMap[m_trackType])) && (m_trackSelector->selectTrack(track))) { 
    /// Clone track
    Track& clonedTrack = *(track->cloneWithKey());
    outputContainer->add(&clonedTrack);
  }
}

void TrackFilterAlg::filterTracks(const Tracks* tracks) {
  Tracks* output = new Tracks();
  for_each(tracks->begin(), tracks->end(), bind(&TrackFilterAlg::filterTrack, this, _1, output));
  /// put filtered track container in TES
  put(output, m_tracksOutputContainer);
}
