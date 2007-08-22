// $Id: PatVeloAddTruth.cpp,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Linker/LinkedTo.h"
#include "Event/VeloCluster.h"
#include "Event/MCParticle.h"
// local
#include "PatVeloAddTruth.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloAddTruth
//
// 2006-10-02 : Olivier Callot
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatVeloAddTruth );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloAddTruth::PatVeloAddTruth( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
PatVeloAddTruth::~PatVeloAddTruth() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatVeloAddTruth::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  PatDataStore* dataStore = tool<PatDataStore>( "PatDataStore" );
  m_veloCoords   = dataStore->veloCoords();
  release( dataStore );

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode PatVeloAddTruth::execute() {

  debug() << "==> Execute" << endmsg;

  LinkedTo<LHCb::MCParticle, LHCb::VeloCluster> veloLink( evtSvc(), msgSvc(), 
                                                          LHCb::VeloClusterLocation::Default );

  for ( std::vector<PatVeloCoord*>::const_iterator it = m_veloCoords->begin();
        m_veloCoords->end() != it; ++it ) {
    PatVeloCoord* hit = const_cast<PatVeloCoord*>( *it );
    LHCb::VeloChannelID id =  hit->veloID();
    LHCb::MCParticle* part = veloLink.first( id );
    while ( 0 != part ) {
      hit->keys().push_back( part->key() );
      part = veloLink.next();
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatVeloAddTruth::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
