// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "FilterDownstreamTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterDownstreamTracks
//
// 2010-06-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterDownstreamTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterDownstreamTracks::FilterDownstreamTracks( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Filter", m_filter = true );
}
//=============================================================================
// Destructor
//=============================================================================
FilterDownstreamTracks::~FilterDownstreamTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterDownstreamTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterDownstreamTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward    = get<LHCb::Tracks>( LHCb::TrackLocation::Forward    );
  LHCb::Tracks* downstream = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream );
  
  debug() << "Comparing " << forward->size() << " forward to " << downstream->size() << " downstream tracks" << endmsg;

  for ( LHCb::Tracks::iterator itForward = forward->begin(); forward->end() != itForward; ++itForward ) {
    for ( LHCb::Tracks::iterator itDown = downstream->begin();  downstream->end() != itDown; ++itDown ) {
      unsigned int nCommon = (*itForward)->nCommonLhcbIDs( **itDown );
      if ( (*itDown)->nLHCbIDs() * 0.8 <= nCommon ) {
        debug() << "Tracks forward " << (*itForward)->key() << " shares hits with downstream track " 
               << (*itDown)->key() << " : nCommon " << nCommon;
        if ( m_filter ) {
          downstream->erase( itDown );
          itDown = downstream->begin()- 1; //== re-initialize the iterator
          debug() << " Downstream track removed.";
        } 
        debug() << endmsg;
      }
    }
  }

  LHCb::Tracks* match = get<LHCb::Tracks>( LHCb::TrackLocation::Match   );
  debug() << "Comparing " << match->size() << " match to " << downstream->size() << " downstream tracks" << endmsg;

  for ( LHCb::Tracks::iterator itMatch = match->begin(); match->end() != itMatch; ++itMatch ) {
    for ( LHCb::Tracks::iterator itDown = downstream->begin();  downstream->end() != itDown; ++itDown ) {
      unsigned int nCommon = (*itMatch)->nCommonLhcbIDs( **itDown );
      if ( (*itDown)->nLHCbIDs() * 0.8 <= nCommon ) {
        debug() << "Tracks match " << (*itMatch)->key() << " shares hits with downstream track " 
               << (*itDown)->key() << " : nCommon " << nCommon;
        if ( m_filter ) {
          downstream->erase( itDown );
          itDown = downstream->begin()- 1; //== re-initialize the iterator
          debug() << " Downstream track removed.";
        } 
        debug() << endmsg;
      }
    }
  }
  debug() << "After filter, rests " << downstream->size() << " downstream tracks to fit" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterDownstreamTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
