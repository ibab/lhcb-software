// Include files 

#include "Event/Track.h"

// local
#include "FilterMatchTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMatchTracks
//
// 2010-06-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterMatchTracks )


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
// Main execution
//=============================================================================
StatusCode FilterMatchTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward = get<LHCb::Tracks>( LHCb::TrackLocation::Forward );
  LHCb::Tracks* match   = get<LHCb::Tracks>( LHCb::TrackLocation::Match   );
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Comparing " << forward->size() << " forward to " << match->size() << " match tracks" << endmsg;

  for ( LHCb::Tracks::iterator itForward = forward->begin(); forward->end() != itForward; ++itForward ) {
    for ( LHCb::Tracks::iterator itMatch = match->begin();  match->end() != itMatch; ++itMatch ) {
      if ( (*itForward)->nLHCbIDs() < (*itMatch)->nLHCbIDs() ) continue;
      LHCb::Track* myMatch = *itMatch;
      unsigned int nCommon = (*itForward)->nCommonLhcbIDs( *myMatch );
      if ( nCommon ==  (*itMatch)->nLHCbIDs() ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Tracks forward " << (*itForward)->key() << " is containing match track " 
                << (*itMatch)->key() << " .";
        if ( m_filter ) {
          match->erase( itMatch );
          itMatch = match->begin() - 1; //== re-initialize the iterator
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << " Match track removed.";
        }
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << endmsg;
      }
    }
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "After filter, rests " << match->size() << " match tracks to fit" << endmsg;


  return StatusCode::SUCCESS;
}
