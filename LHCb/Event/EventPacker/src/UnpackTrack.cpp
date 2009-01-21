// $Id: UnpackTrack.cpp,v 1.5 2009-01-21 14:17:53 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/StandardPacker.h"

// local
#include "UnpackTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackTrack
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackTrack );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTrack::UnpackTrack( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedTrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::TrackLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackTrack::~UnpackTrack() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackTrack::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::PackedTracks* dst = get<LHCb::PackedTracks>( m_inputName );
  debug() << "Size of PackedTracks = " << dst->end() - dst->begin() << endreq;

  LHCb::Tracks* newTracks = new LHCb::Tracks();
  put( newTracks, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedTrack>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedTrack& src = (*itS);

    LHCb::Track* track = new LHCb::Track( );
    newTracks->insert( track, src.key );

    track->setChi2PerDoF( pack.fltPacked( src.chi2PerDoF ) );
    track->setNDoF(       src.nDoF );
    track->setFlags(      src.flags );
    for ( int kId = src.firstId; src.lastId > kId; ++kId ) {
      unsigned int id = *(dst->beginIds()+kId);
      track->addToLhcbIDs( LHCb::LHCbID( id ) );
    }
    for ( int kSt = src.firstState; src.lastState > kSt; ++kSt ) {
      LHCb::PackedState pSta = *(dst->beginState()+kSt);
      convertState( pSta, track );
    }
    for ( int kEx = src.firstExtra; src.lastExtra > kEx; ++kEx ) {
      std::pair<int,int> info = *(dst->beginExtra()+kEx);
      track->addInfo( info.first, pack.fltPacked( info.second ) );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Convert one state
//=========================================================================
void UnpackTrack::convertState ( const LHCb::PackedState& src, LHCb::Track* tra ) {
  LHCb::StateVector     stateVector;
  Gaudi::TrackSymMatrix stateCov;

  StandardPacker pack;
    
  stateVector.setX( pack.position( src.x ) );
  stateVector.setY( pack.position( src.y ) );
  stateVector.setZ( pack.position( src.z ) );
  stateVector.setTx( pack.slope( src.tx ) );
  stateVector.setTy( pack.slope( src.ty ) );
  double pInv = 0.;
  if ( 0 != src.p )  pInv =  1./pack.energy( src.p );
  stateVector.setQOverP( pInv );

  //== Fill covariance matrix
  double err0 = pack.position( src.cov_00  );
  double err1 = pack.position( src.cov_11  );
  double err2 = pack.slope(    src.cov_22  );
  double err3 = pack.slope(    src.cov_33  );
  double err4 = pack.energy(   src.cov_44  ) * fabs(pInv) * 1.e-5;
  
  stateCov(0,0) = err0 * err0;
  stateCov(1,1) = err1 * err1;
  stateCov(2,2) = err2 * err2;
  stateCov(3,3) = err3 * err3;
  stateCov(4,4) = err4 * err4;
  stateCov(1,0) = err1 * err0 * pack.fraction( src.cov_10 );
  stateCov(2,0) = err2 * err0 * pack.fraction( src.cov_20 );
  stateCov(2,1) = err2 * err1 * pack.fraction( src.cov_21 );
  stateCov(3,0) = err3 * err0 * pack.fraction( src.cov_30 );
  stateCov(3,1) = err3 * err1 * pack.fraction( src.cov_31 );
  stateCov(3,2) = err3 * err2 * pack.fraction( src.cov_32 );
  stateCov(4,0) = err4 * err0 * pack.fraction( src.cov_40 );
  stateCov(4,1) = err4 * err1 * pack.fraction( src.cov_41 );
  stateCov(4,2) = err4 * err2 * pack.fraction( src.cov_42 );
  stateCov(4,3) = err4 * err3 * pack.fraction( src.cov_43 );

  LHCb::State state( stateVector );
  state.setCovariance( stateCov );
  state.setFlags( src.flags );
  tra->addToStates( state );
}
//=============================================================================
