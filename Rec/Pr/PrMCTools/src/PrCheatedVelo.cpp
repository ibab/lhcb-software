// Include files 

#include "Event/MCHit.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "Event/VeloCluster.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/MCTrackInfo.h"
// local
#include "PrCheatedVelo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrCheatedVelo
//
// 2012-07-26 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrCheatedVelo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrCheatedVelo::PrCheatedVelo( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
 
}
//=============================================================================
// Destructor
//=============================================================================
PrCheatedVelo::~PrCheatedVelo() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrCheatedVelo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrCheatedVelo::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Tracks* vTracks = new LHCb::Tracks();
  put( vTracks, LHCb::TrackLocation::Velo );
  
  const LHCb::MCParticles* partCont = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  LinkedFrom<ContainedObject,LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  LHCb::MCHits* vHits = get<LHCb::MCHits>( LHCb::MCHitLocation::Velo );
   
  LHCb::MCParticles::const_iterator itP = partCont->begin();
  for (; itP != partCont->end(); ++itP){
    LHCb::MCParticle* part = *itP;
    if ( 0 == trackInfo.fullInfo( part ) ) continue;
    if ( !trackInfo.hasVelo( part ) ) continue;
    std::vector<int> range = vLink.keyRange( part );
    LHCb::Track* vTr = new LHCb::Track;
    for ( std::vector<int>::iterator itI = range.begin(); range.end() != itI; ++itI ) {
      LHCb::VeloChannelID vId( *itI );
      vTr->addToLhcbIDs( LHCb::LHCbID( vId ) );
    }
    //== build a state from MCHits
    std::vector<Gaudi::XYZPoint> vPoints;  
    for ( LHCb::MCHits::const_iterator vHitIt = vHits->begin() ;
          vHits->end() != vHitIt ; vHitIt++ ) {
      if ( (*vHitIt)->mcParticle() ==  part ) {
        vPoints.push_back( (*vHitIt)->midPoint() );
      }
    }
    double m_zVelo = 0.;
    double dz = 0.;
    PrFitPolinomial fitX( 1 );
    PrFitPolinomial fitY( 1 );
    for ( std::vector<Gaudi::XYZPoint>::iterator pt = vPoints.begin() ; 
          vPoints.end() != pt  ; pt++ ) {
      dz = (*pt).z() - m_zVelo;
      fitX.fill(  (*pt).x(), dz );
      fitY.fill(  (*pt).y(), dz );
    }
    fitX.solve();
    fitY.solve();
    
    LHCb::State vState;
    vState.setLocation( LHCb::State::ClosestToBeam );
    vState.setState( fitX.param(0), fitY.param(0), m_zVelo, fitX.param(1), fitY.param(1), 0. );
    vTr->addToStates( vState );
    if ( 0 > part->momentum().z() ) vTr->setFlag( LHCb::Track::Backward, true );
    vTracks->insert( vTr );
  }
  return StatusCode::SUCCESS;
}

  //===========================================================================
//  Finalize
//=============================================================================
StatusCode PrCheatedVelo::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
