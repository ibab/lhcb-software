// $Id: PackTrack.cpp,v 1.11 2009-10-14 16:22:02 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Kernel/StandardPacker.h"

// local
#include "PackTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTrack
//
// 2008-11-12 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackTrack );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackTrack::PackTrack( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedTrackLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackTrack::~PackTrack() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTrack::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Tracks* tracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_inputName );
  LHCb::PackedTracks* out = new LHCb::PackedTracks();
  put( out, m_outputName );
  out->setVersion( 3 );

  StandardPacker pack;
  
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
    LHCb::PackedTrack newTrk;
    LHCb::Track* track = *itT;

    newTrk.key        = track->key();
    newTrk.chi2PerDoF = pack.fltPacked( track->chi2PerDoF() );
    newTrk.nDoF       = track->nDoF();
    newTrk.flags      = track->flags();
    newTrk.likelihood = pack.fltPacked( track->likelihood() );
    newTrk.ghostProba = pack.fltPacked( track->ghostProbability() );
    
    //== Store the LHCbID as int 
    newTrk.firstId    = out->sizeId();
    for ( std::vector<LHCb::LHCbID>::const_iterator itI = track->lhcbIDs().begin();
          track->lhcbIDs().end() != itI; ++itI ) {
      out->addId((*itI).lhcbID() );
    }
    newTrk.lastId    = out->sizeId();
    debug() << "Stored LHCbIDs from " << newTrk.firstId << " to " << newTrk.lastId << endmsg;

    //== Handle the states in the track
    newTrk.firstState = out->sizeState();
    for ( std::vector<LHCb::State*>::const_iterator itS = track->states().begin();  track->states().end() != itS; ++itS ) {
      convertState( *itS, out );
    }
    newTrk.lastState = out->sizeState();
    debug() << "Stored states from " << newTrk.firstState << " to " << newTrk.lastState << endmsg;

    //== Handles the ExtraInfo
    newTrk.firstExtra = out->sizeExtra();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = track->extraInfo().begin();
          track->extraInfo().end() != itE; ++itE ) {
      out->addExtra( (*itE).first, pack.fltPacked( (*itE).second ) );
    }
    newTrk.lastExtra = out->sizeExtra();

    out->addEntry( newTrk );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  tracks->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Convert a track state
//=========================================================================
void PackTrack::convertState ( const LHCb::State* state, LHCb::PackedTracks* out) {
      
  LHCb::PackedState newState;
  StandardPacker pack;

  newState.flags = state->flags();

  newState.x    = pack.position( state->x()  );
  newState.y    = pack.position( state->y()  );
  newState.z    = pack.position( state->z()  );
  newState.tx   = pack.slope   ( state->tx() );
  newState.ty   = pack.slope   ( state->ty() );
  double p = 0.;
  if ( 0 != state->qOverP() ) p = 1./ state->qOverP();
  newState.p    = pack.energy  ( p );
  p = pack.energy( newState.p );  //== Get the coded value in case of saturation, to code properly the error later
  
  // convariance Matrix
  std::vector<double> err;
  err.push_back( sqrt( state->errX2() ) );
  err.push_back( sqrt( state->errY2() ) );
  err.push_back( sqrt( state->errTx2() ) );
  err.push_back( sqrt( state->errTy2() ) );
  err.push_back( sqrt( state->errQOverP2() ) );
  
  newState.cov_00 = pack.position( err[0] );
  newState.cov_11 = pack.position( err[1] );
  newState.cov_22 = pack.slope   ( err[2] );
  newState.cov_33 = pack.slope   ( err[3] );
  newState.cov_44 = pack.energy  ( 1.e5 * fabs(p) * err[4] ); //== 1.e5 * dp/p (*1.e2)

  newState.cov_10 = pack.fraction( state->covariance()(1,0)/err[1]/err[0] );
  newState.cov_20 = pack.fraction( state->covariance()(2,0)/err[2]/err[0] );
  newState.cov_21 = pack.fraction( state->covariance()(2,1)/err[2]/err[1] );
  newState.cov_30 = pack.fraction( state->covariance()(3,0)/err[3]/err[0] );
  newState.cov_31 = pack.fraction( state->covariance()(3,1)/err[3]/err[1] );
  newState.cov_32 = pack.fraction( state->covariance()(3,2)/err[3]/err[2] );
  newState.cov_40 = pack.fraction( state->covariance()(4,0)/err[4]/err[0] );
  newState.cov_41 = pack.fraction( state->covariance()(4,1)/err[4]/err[1] );
  newState.cov_42 = pack.fraction( state->covariance()(4,2)/err[4]/err[2] );
  newState.cov_43 = pack.fraction( state->covariance()(4,3)/err[4]/err[3] );
  
  out->addState( newState );
}

//=============================================================================
