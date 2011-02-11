// $Id: UnpackTrack.cpp,v 1.15 2009-12-12 15:37:20 jonrob Exp $
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
DECLARE_ALGORITHM_FACTORY( UnpackTrack )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTrack::UnpackTrack( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedTrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::TrackLocation::Default );
  declareProperty( "AlwaysCreateOutput",      m_alwaysOutput = false     );
  declareProperty( "AncestorFor",             m_ancestorFor    = LHCb::PackedTrackLocation::Muon );
  declareProperty( "AncestorSource",          m_ancestorSource = LHCb::TrackLocation::Default );
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

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedTracks>(m_inputName) ) return StatusCode::SUCCESS;

  const LHCb::PackedTracks* dst = getOrCreate<LHCb::PackedTracks,LHCb::PackedTracks>( m_inputName );
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Size of PackedTracks = " << dst->end() - dst->begin() << endmsg;

  LHCb::Tracks* newTracks = new LHCb::Tracks();
  newTracks->reserve( dst->tracks().size() );
  put( newTracks, m_outputName );

  //== If one nedds to build ancestors, get the location of the ancestor's container
  LHCb::Tracks* targetTracks = 0;
  if ( m_inputName == m_ancestorFor ) {
    targetTracks = get<LHCb::Tracks>( m_ancestorSource );
  }
  
  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedTrack>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedTrack& src = (*itS);

    LHCb::Track* track = new LHCb::Track( );
    newTracks->insert( track, src.key );

    track->setChi2PerDoF( pack.fltPacked( src.chi2PerDoF ) );
    track->setNDoF(       src.nDoF );
    track->setFlags(      src.flags );
    if ( dst->version() > 2 ) {
      track->setLikelihood(       pack.fltPacked( src.likelihood ) );
      track->setGhostProbability( pack.fltPacked( src.ghostProba ) );
    }
    std::vector<LHCb::LHCbID> lhcbids(  src.lastId - src.firstId  ) ;
    std::vector<LHCb::LHCbID>::iterator lhcbit = lhcbids.begin() ;
    for ( int kId = src.firstId; src.lastId > kId; ++kId, ++lhcbit ) {
      unsigned int id = *(dst->beginIds()+kId);
      *lhcbit = LHCb::LHCbID( id ) ;
    }
    // schema change: sorting no longer needed when we write DSTs with sorted lhcbids
    if( dst->version() <= 1 ) {
      std::sort( lhcbids.begin(), lhcbids.end() ) ;
    }
    track->addSortedToLhcbIDs( lhcbids ) ;

    for ( int kSt = src.firstState; src.lastState > kSt; ++kSt ) {
      LHCb::PackedState pSta = *(dst->beginState()+kSt);
      convertState( pSta, track );
    }
    for ( int kEx = src.firstExtra; src.lastExtra > kEx; ++kEx ) {
      std::pair<int,int> info = *(dst->beginExtra()+kEx);
      track->addInfo( info.first, pack.fltPacked( info.second ) );
    }
    //== Cleanup extraInfo and set likelihood/ghostProbability for old data
    if ( dst->version() <= 2 ) {
      track->eraseInfo(LHCb::Track::PatQuality);
      track->eraseInfo(LHCb::Track::Cand1stQPat);
      track->eraseInfo(LHCb::Track::Cand2ndQPat);
      track->eraseInfo(LHCb::Track::NCandCommonHits);
      track->eraseInfo(LHCb::Track::Cand1stChi2Mat);
      track->eraseInfo(LHCb::Track::Cand2ndChi2Mat); 
      track->eraseInfo(LHCb::Track::MatchChi2);
      track->eraseInfo(LHCb::Track::TsaLikelihood); 
      track->eraseInfo(LHCb::Track::nPRVeloRZExpect);
      track->eraseInfo(LHCb::Track::nPRVelo3DExpect);
      track->setLikelihood(       track->info(   1, 999) );        // was the key of likelihood...
      track->setGhostProbability( track->info( 102, 999) );        // was the key of ghost probability
      track->eraseInfo(   1 );
      track->eraseInfo( 102 );
    }
    //== Create an ancestor if needed
    if ( m_inputName == m_ancestorFor ) {
      LHCb::Track* ancest = targetTracks->object( track->key() );
      if ( 0 != ancest ) {   
        SmartRef<LHCb::Track> myAncestor( ancest );
        track->addToAncestors( ancest );
        debug() << "Add ancestor for track " << track->key() << endmsg;
      }
    }
  }

  counter("# UnpackedTracks") += newTracks->size();

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
