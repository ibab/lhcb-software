// $Id: TrackFilterAlg.cpp,v 1.12 2008-05-30 13:29:12 lnicolas Exp $
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
    m_trackSelector(0)
{
  /// Map strings to track types
  m_stringToTrackTypeMap = map_list_of("Velo"      , Track::Velo      )
                                      ("VeloR"     , Track::VeloR     )
                                      ("Long"      , Track::Long      )
                                      ("Upstream"  , Track::Upstream  )
                                      ("Downstream", Track::Downstream)
                                      ("Ttrack"    , Track::Ttrack    )
                                      ("Muon"      , Track::Muon      );

  /// And vice versa. I whish we had Boost::BiMap
  m_trackTypeToStringMap = map_list_of(Track::Velo      , "Velo"      )
                                      (Track::VeloR     , "VeloR"     )
                                      (Track::Long      , "Long"      )
                                      (Track::Upstream  , "Upstream"  )
                                      (Track::Downstream, "Downstream")
                                      (Track::Ttrack    , "Ttrack"    )
                                      (Track::Muon      , "Muon"      );

  m_lhcbDetChecks = map_list_of("Velo", boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isVelo,_1)))
                               ("TT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isTT  ,_1)))
                               ("IT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isIT  ,_1)))
                               ("OT"  , boost::function<bool (LHCb::LHCbID)>(bind<bool>(&LHCb::LHCbID::isOT  ,_1)));

  declareProperty("TracksInputContainer"     , m_tracksInputContainer  = TrackLocation::Default    );
  declareProperty("TracksOutputContainer"    , m_tracksOutputContainer = "Alignment/FilteredTracks");
  declareProperty("TrackType"                , m_trackType             = "Long"                    );
  declareProperty("TrackSelector"            , m_trackSelectorName     = "AlignSelTool"            );
  declareProperty("StripUnwantedDetectorHits", m_strip                 = false                     );
  declareProperty("KeepDetectorHits"         , m_detector              = "OT"                      );
  declareProperty("MinNHits"                 , m_nMinHits              = 0u                        );

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
  m_trackSelector = tool<ITrackSelector>(m_trackSelectorName, "Selector", this);
  if (!m_trackSelector) return Error("==> Failed to retrieve track selector tool", StatusCode::FAILURE);

  m_elementsToBeAligned = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_elementsToBeAligned) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
  m_rangeElements = m_elementsToBeAligned->rangeElements();
  
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
  if ((track->checkType(m_stringToTrackTypeMap[m_trackType])) && (m_trackSelector->accept(*track))) {
    /// Clone track. It's mine
    std::auto_ptr<Track> clonedTrack( track->cloneWithKey() );
    /// let's strip unwanted nodes. This should be a seperate algorithm
    /// Get my own copy of nodes
    LHCBIDS ids = clonedTrack->lhcbIDs();
    if (printDebug()) {
      debug() << "==> Got " << ids.size() << " ids " <<endmsg;
    }
    if (m_strip) {
      debug() << "==> Stripping unwanted ids" << endmsg;
      for_each(ids.begin(), ids.end(), bind(&TrackFilterAlg::strip, this, _1, clonedTrack.get()));
    }
    if (printDebug()) {
      const LHCBIDS& wantedIds = clonedTrack->lhcbIDs();
      debug() << wantedIds.size() << " ids left after stripping" << endmsg;
      int nID = 0;
      for (LHCBIDS::const_iterator i = wantedIds.begin(), iEnd = wantedIds.end(); i != iEnd; ++nID, ++i) {
        debug() << "==> " << nID << ": ID is " << (*i) << endmsg;
      }
    }

    unsigned nHits = 0u;
    const LHCBIDS& wantedIds = clonedTrack->lhcbIDs();
    for (LHCBIDS::const_iterator i = wantedIds.begin(), iEnd = wantedIds.end(); i != iEnd; ++i) {
      const AlignmentElement* elem = m_elementsToBeAligned->findElement((*i));
      if (!elem) {
        if (printDebug()) debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
      ++nHits;
    }

    if ( nHits >= m_nMinHits )  {
      if (printDebug()) {
        debug() << "Found track of type " << m_trackTypeToStringMap[track->type()] << " with a chi2/dof: " << track->chi2() << " / " << track->nDoF() 
                << " and " << nHits << " hits of type " << m_detector << endmsg;
      }
                          /// It's yours              
      outputContainer->add(clonedTrack.release());
    }
  }
}

void TrackFilterAlg::filterTracks(const Tracks* tracks) {
  Tracks* output = new Tracks();
  for_each(tracks->begin(), tracks->end(), bind(&TrackFilterAlg::filterTrack, this, _1, output));
  /// put filtered track container in TES
  put(output, m_tracksOutputContainer);
}
