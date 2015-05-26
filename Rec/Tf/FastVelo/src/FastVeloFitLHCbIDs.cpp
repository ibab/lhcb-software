// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Track.h"
// local
#include "FastVeloFitLHCbIDs.h"
#include "FastVeloTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloFitLHCbIDs
//
// 2010-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FastVeloFitLHCbIDs )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVeloFitLHCbIDs::FastVeloFitLHCbIDs( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackFitter>(this);
  declareProperty( "StateAtBeam"  , m_stateAtBeam = true );
}
//=============================================================================
// Destructor
//=============================================================================
FastVeloFitLHCbIDs::~FastVeloFitLHCbIDs() {} 

//=========================================================================
//  Initialization
//=========================================================================
StatusCode FastVeloFitLHCbIDs::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );
  m_hitManager  = tool<FastVeloHitManager>( "FastVeloHitManager", "FastVeloHitManager" );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Fit a single track. The second argument is ignored.
//=========================================================================
StatusCode FastVeloFitLHCbIDs::fit( LHCb::Track & track, LHCb::ParticleID) {

  int nRight(0), nLeft(0);
  std::vector< LHCb::LHCbID > otherIDs;
  FastVeloHits phiHits;
  phiHits.reserve(20);
  std::vector<int> byZone(8,0);
  FastVeloTrack fastTrack;
  double zMin =  1.e9;
  double zMax = -1.e9;

  std::vector<LHCb::LHCbID>::const_iterator iID;
  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    if( iID->isVelo() ){
      FastVeloHit* hit = m_hitManager->defaultStateHitByLHCbID( *iID );
      if ( 0 == hit ) {
        error() << "*** Hit not found, id " << *iID << endmsg;
      } else {
	unsigned int zone = hit->zone();
	if ( m_velo->sensor(iID->veloID().sensor())->isRight() ) {
	  ++nRight;
	  zone += 4;
	}else{
	  ++nLeft;
	}
	if ( hit->z() > zMax ) zMax = hit->z();
	if ( hit->z() < zMin ) zMin = hit->z();
        if ( iID->isVeloR() ) {
          ++byZone[ zone ];
          fastTrack.addRHit( hit );
        } else {
          phiHits.push_back( hit);
        }
      }  
    } else {
      otherIDs.push_back( *iID );
    }
  }

  // set phi weights as a function of track radius
  for(auto &pHit : phiHits){
    pHit->setPhiWeight(fastTrack.rPred(pHit->z()));
  }

  //== Get the zone
  int maxZone = 0;
  for ( unsigned int kk = 0 ; byZone.size() > kk ; ++kk ) {
    if ( byZone[kk] > byZone[maxZone] ) maxZone = kk;
  }

  double cosPhi, sinPhi;
  if( nRight==0 || nLeft==0 ){
    sinPhi = m_hitManager->sinPhi( maxZone );
    cosPhi = m_hitManager->cosPhi( maxZone );
  }else{
    //overlap track so phi is average of zone 0 & 7 or 3 & 4
    if( maxZone == 3 || maxZone == 4 ){
      sinPhi = (m_hitManager->sinPhi(3) + m_hitManager->sinPhi(4)) / 2.;
      cosPhi = (m_hitManager->cosPhi(3) + m_hitManager->cosPhi(4)) / 2.;
    }else{ // if not 3 & 4 it must be zone 0 & 7
      sinPhi = (m_hitManager->sinPhi(0) + m_hitManager->sinPhi(7)) / 2.;
      cosPhi = (m_hitManager->cosPhi(0) + m_hitManager->cosPhi(7)) / 2.;
    }      
  }

  fastTrack.setSpaceParametersFromR( cosPhi, sinPhi );
  fastTrack.updateRParameters(); // in case in overlap and need correcting
  fastTrack.addPhiClusters( phiHits );
  if( !fastTrack.testStateFit() ){
    // oops the internal fit failed: 
    // track created in second phi pass: needed to correct R cluster to phi first
    fastTrack.setSpaceParametersFromR( cosPhi, sinPhi );
    double xSeed(0.),ySeed(0.);
    for(auto &pHit : phiHits){
      xSeed += pHit->xStripCentre();
      ySeed += pHit->yStripCentre();
    }
    xSeed /= phiHits.size();
    ySeed /= phiHits.size();
    for(auto &rHit : fastTrack.rHits()){
      rHit->setStartingPoint( xSeed, ySeed );
    }
  }
  fastTrack.updateRParameters();
  double zBeam = fastTrack.zBeam();

  LHCb::State state;
  if ( m_stateAtBeam ) {
    state.setLocation( LHCb::State::ClosestToBeam );
    state.setState( fastTrack.state( zBeam ) );
    state.setCovariance( fastTrack.covariance( zBeam ) );
  } else {
    state.setLocation( LHCb::State::FirstMeasurement );
    state.setState( fastTrack.state( zMin ) );
    state.setCovariance( fastTrack.covariance( zMin ) );
  }
  track.addToStates( state );

  if ( !track.checkFlag( LHCb::Track::Backward) ) {
    state.setLocation( LHCb::State::EndVelo );
    state.setState( fastTrack.state( zMax ) );
    state.setCovariance( fastTrack.covariance( zMax ) );
    track.addToStates( state );
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
