// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "FilterSeedTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterSeedTracks
//
// 2010-06-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterSeedTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterSeedTracks::FilterSeedTracks( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Filter", m_filter = true );
}
//=============================================================================
// Destructor
//=============================================================================
FilterSeedTracks::~FilterSeedTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterSeedTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterSeedTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward = get<LHCb::Tracks>( LHCb::TrackLocation::Forward    );
  LHCb::Tracks* seed    = get<LHCb::Tracks>( LHCb::TrackLocation::Seed );
  
  debug() << "Comparing " << forward->size() << " forward to " << seed->size() << " seed tracks" << endmsg;

  for ( LHCb::Tracks::iterator itForward = forward->begin(); forward->end() != itForward; ++itForward ) {
    for ( LHCb::Tracks::iterator itSeed = seed->begin();  seed->end() != itSeed; ++itSeed ) {
      unsigned int nCommon = (*itForward)->nCommonLhcbIDs( **itSeed );
      if ( (*itSeed)->nLHCbIDs() == nCommon ) {
        debug() << "Seed track " << (*itSeed)->key() << " shares all its "
                << nCommon << " hits with forward track " << (*itForward)->key();
        if ( m_filter ) {
          seed->erase( itSeed );
          itSeed = seed->begin()- 1; //== re-initialize the iterator
          debug() << " Seed track removed.";
        } 
        debug() << endmsg;
      }
    }
  }

  LHCb::Tracks* match = get<LHCb::Tracks>( LHCb::TrackLocation::Match    );
  debug() << "Comparing " << match->size() << " match to " << seed->size() << " seed tracks" << endmsg;

  for ( LHCb::Tracks::iterator itMatch = match->begin(); match->end() != itMatch; ++itMatch ) {
    for ( LHCb::Tracks::iterator itSeed = seed->begin();  seed->end() != itSeed; ++itSeed ) {
      unsigned int nCommon = (*itMatch)->nCommonLhcbIDs( **itSeed );
      if ( (*itSeed)->nLHCbIDs() == nCommon ) {
        debug() << "Seed track " << (*itSeed)->key() << " shares all its "
                << nCommon << " hits with match track " << (*itMatch)->key();
        if ( m_filter ) {
          seed->erase( itSeed );
          itSeed = seed->begin()- 1; //== re-initialize the iterator
          debug() << " Seed track removed.";
        } 
        debug() << endmsg;
      }
    }
  }

  LHCb::Tracks* downstream = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream    );
  debug() << "Comparing " << downstream->size() << " downstream to " << seed->size() << " seed tracks" << endmsg;

  for ( LHCb::Tracks::iterator itDownstream = downstream->begin(); downstream->end() != itDownstream; ++itDownstream ) {
    for ( LHCb::Tracks::iterator itSeed = seed->begin();  seed->end() != itSeed; ++itSeed ) {
      unsigned int nCommon = (*itDownstream)->nCommonLhcbIDs( **itSeed );
      if ( (*itSeed)->nLHCbIDs() == nCommon ) {
        debug() << "Seed track " << (*itSeed)->key() << " shares all its "
                << nCommon << " hits with downstream track " << (*itDownstream)->key();
        if ( m_filter ) {
          seed->erase( itSeed );
          itSeed = seed->begin()- 1; //== re-initialize the iterator
          debug() << " Seed track removed.";
        } 
        debug() << endmsg;
      }
    }
  }



  debug() << "After filter, rests " << seed->size() << " seed tracks to fit" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterSeedTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
