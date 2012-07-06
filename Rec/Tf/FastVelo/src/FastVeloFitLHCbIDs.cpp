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
  declareProperty( "MSFactor"     , m_msFactor    = 0.95 );
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
  std::vector<int> byZone(4,0);
  FastVeloTrack fastTrack;
  double zMin =  1.e9;
  double zMax = -1.e9;

  std::vector<LHCb::LHCbID>::const_iterator iID;
  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    if( iID->isVelo() ){
      if ( m_velo->sensor(iID->veloID().sensor())->isRight() ) {
        ++nRight;
      }else{
        ++nLeft;
      }
      FastVeloHit* hit = m_hitManager->hitByLHCbID( *iID );
      if ( 0 == hit ) {
        error() << "*** Hit not found, id " << *iID << endmsg;
     } else {
	if ( hit->z() > zMax ) zMax = hit->z();
	if ( hit->z() < zMin ) zMin = hit->z();
        if ( iID->isVeloR() ) {
          ++byZone[ hit->zone() ];
          fastTrack.addRHit( hit );
        } else {
          phiHits.push_back( hit);
        }
      }  
    } else {
      otherIDs.push_back( *iID );
    }
  }
  //== Get the zone
  int maxZone = 0;
  for ( int kk = 0 ; 4 > kk ; ++kk ) {
    if ( byZone[kk] > byZone[maxZone] ) maxZone = kk;
  }
  if ( nRight > nLeft ) maxZone += 4;

  fastTrack.setSpaceParametersFromR( m_hitManager->cosPhi( maxZone ), m_hitManager->sinPhi( maxZone ) );
  fastTrack.addPhiClusters( phiHits );
  fastTrack.updateRParameters();

  fastTrack.fitTrack( );
  
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
