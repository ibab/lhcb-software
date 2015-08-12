// Include files 

#include "Event/Track.h"

// local
#include "FilterDownstreamTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterDownstreamTracks
//
// 2010-06-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterDownstreamTracks )


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
// Main execution
//=============================================================================
StatusCode FilterDownstreamTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward    = get<LHCb::Tracks>( LHCb::TrackLocation::Forward    );
  LHCb::Tracks* downstream = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream );
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Comparing " << forward->size() << " forward to " << downstream->size() << " downstream tracks" << endmsg;

  for ( LHCb::Tracks::iterator itForward = forward->begin(); forward->end() != itForward; ++itForward ) {
    for ( LHCb::Tracks::iterator itDown = downstream->begin();  downstream->end() != itDown; ++itDown ) {
      unsigned int nCommon = (*itForward)->nCommonLhcbIDs( **itDown );
      if ( (*itDown)->nLHCbIDs() * 0.8 <= nCommon ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Tracks forward " << (*itForward)->key() << " shares hits with downstream track " 
                  << (*itDown)->key() << " : nCommon " << nCommon;
        if ( m_filter ) {
          downstream->erase( itDown );
          itDown = downstream->begin()- 1; //== re-initialize the iterator
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << " Downstream track removed.";
        } 
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << endmsg;
      }
    }
  }

  LHCb::Tracks* match = get<LHCb::Tracks>( LHCb::TrackLocation::Match   );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Comparing " << match->size() << " match to " << downstream->size() << " downstream tracks" << endmsg;

  for ( LHCb::Tracks::iterator itMatch = match->begin(); match->end() != itMatch; ++itMatch ) {
    for ( LHCb::Tracks::iterator itDown = downstream->begin();  downstream->end() != itDown; ++itDown ) {
      unsigned int nCommon = (*itMatch)->nCommonLhcbIDs( **itDown );
      if ( (*itDown)->nLHCbIDs() * 0.8 <= nCommon ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Tracks match " << (*itMatch)->key() << " shares hits with downstream track " 
               << (*itDown)->key() << " : nCommon " << nCommon;
        if ( m_filter ) {
          downstream->erase( itDown );
          itDown = downstream->begin()- 1; //== re-initialize the iterator
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << " Downstream track removed.";
        } 
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << endmsg;
      }
    }
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "After filter, rests " << downstream->size() << " downstream tracks to fit" << endmsg;
  return StatusCode::SUCCESS;
}
