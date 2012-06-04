// $Id$
// Include files 
#include "Event/MCHit.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TestFTHits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestFTHits
//
// 2012-05-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestFTHits )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestFTHits::TestFTHits( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
 
}
//=============================================================================
// Destructor
//=============================================================================
TestFTHits::~TestFTHits() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestFTHits::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestFTHits::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::MCHits* hits = get<LHCb::MCHits>( "MC/FT/Hits" );
  for ( LHCb::MCHits::iterator itH = hits->begin(); hits->end() != itH; ++itH ) {
    info() << format( "x %9.3f y%9.3f z%10.3f dz %7.3f MCkey %4d", 
                      (*itH)->entry().x(), (*itH)->entry().y(), (*itH)->entry().z(),
                      (*itH)->exit().z() - (*itH)->entry().z(),
                      (*itH)->mcParticle()->key() ) << endmsg;
    
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestFTHits::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
