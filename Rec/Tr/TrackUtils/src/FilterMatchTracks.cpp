// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "FilterMatchTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMatchTracks
//
// 2010-06-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterMatchTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterMatchTracks::FilterMatchTracks( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Filter", m_filter = true );
}
//=============================================================================
// Destructor
//=============================================================================
FilterMatchTracks::~FilterMatchTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterMatchTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterMatchTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward = get<LHCb::Tracks>( LHCb::TrackLocation::Forward );
  LHCb::Tracks* match   = get<LHCb::Tracks>( LHCb::TrackLocation::Match   );
  
  debug() << "Comparing " << forward->size() << " forward to " << match->size() << " match tracks" << endmsg;

  for ( LHCb::Tracks::iterator itForward = forward->begin(); forward->end() != itForward; ++itForward ) {
    for ( LHCb::Tracks::iterator itMatch = match->begin();  match->end() != itMatch; ++itMatch ) {
      if ( (*itForward)->nLHCbIDs() < (*itMatch)->nLHCbIDs() ) continue;
      LHCb::Track* myMatch = *itMatch;
      unsigned int nCommon = (*itForward)->nCommonLhcbIDs( *myMatch );
      if ( nCommon ==  (*itMatch)->nLHCbIDs() ) {
        debug() << "Tracks forward " << (*itForward)->key() << " is containing match track " 
                << (*itMatch)->key() << " .";
        if ( m_filter ) {
          match->erase( itMatch );
          itMatch = match->begin() - 1; //== re-initialize the iterator
          debug() << " Match track removed.";
        }
        debug() << endmsg;
      }
    }
  }
  debug() << "After filter, rests " << match->size() << " match tracks to fit" << endmsg;


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterMatchTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
