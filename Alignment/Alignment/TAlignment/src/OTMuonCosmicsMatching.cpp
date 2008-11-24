// $Id: OTMuonCosmicsMatching.cpp,v 1.2 2008-11-24 10:01:30 janos Exp $
// Include files 

// from STD
#include <functional>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/Track.h"
#include "Event/State.h"

// BOOST
#include "boost/foreach.hpp"
#include "boost/lambda/bind.hpp"

// local
#include "OTMuonCosmicsMatching.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTMuonCosmicsMatching
//
// 2008-10-28 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost::lambda;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMuonCosmicsMatching );

OTMuonCosmicsMatching::OTMuonCosmicsMatching( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "TTracksLocation"     , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "MuonTracksLocation"  , m_mTracksLocation      = TrackLocation::Muon        );
  declareProperty( "TracksOutputLocation", m_tracksOutputLocation = m_tTracksLocation+"/TMuon" );
  declareProperty( "Extrapolator"        , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "Chi2Calculator"      , m_nChi2Calculator      = "TrackChi2Calculator"      );
  declareProperty( "MatchAtZ"            , m_matchAtZ             = 12500*Gaudi::Units::mm     );
  declareProperty( "MatchChi2Cut"        , m_matchChi2Cut         = 20.0                       );
  declareProperty( "AllCombinations"     , m_allCombinations      = true                       );
}

OTMuonCosmicsMatching::~OTMuonCosmicsMatching() {} 

StatusCode OTMuonCosmicsMatching::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_extrapolator   = tool<ITrackExtrapolator>(   m_nExtrapolator  , "Extrapolator"  , this );
  m_chi2Calculator = tool<ITrackChi2Calculator>( m_nChi2Calculator, "Chi2Calculator", this );

  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::execute() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_matchTStates.clear();
  m_matchMuonStates.clear();

  const Tracks*       tTracks = get<Tracks>( m_tTracksLocation );
  const Tracks*       mTracks = get<Tracks>( m_mTracksLocation );
  Tracks*       matchedTracks = new Tracks();
  put( matchedTracks, m_tracksOutputLocation );

  //  if ( fullDetail() ) {
  //     plot1D( mTracks->size(), 100, "Number of Muon Tracks to Match" , -0.5, 10.5, 11 );
  //     plot1D( tTracks->size(), 101, "Number of TTracks to Match"     , -0.5, 10.5, 11 );
  //   }
    
  std::vector< double > matchChi2s   ;
  unsigned              nMatches = 0u;

  for ( Tracks::const_iterator t = tTracks->begin(), tEnd = tTracks->end(); t != tEnd; ++t ) {
    double      matchedChi2 = -9998.0;
    Track* matchedMuonTrack =       0;
    for ( Tracks::const_iterator m = mTracks->begin(), mEnd = mTracks->end(); m != mEnd; ++m ) {
      double     chi2 = -9999.0;
      StatusCode   sc = matchChi2( *(*t), *(*m), m_matchAtZ, chi2 );
      if ( sc.isSuccess() ) 
        /// Select one with the best match chi2
        if ( chi2 > 0.0 && ( chi2 < matchedChi2 || m_allCombinations ) ) {
          matchedChi2      = chi2;
          matchedMuonTrack = (*m);
        }
    }
    /// Apply match chi2 selection cut
    if ( matchedChi2 > 0.0 && matchedChi2 < m_matchChi2Cut ) {
      ++nMatches;
      //plot( matchedChi2, 102, "Match chi2", 0, 100, 100 );
      matchChi2s.push_back( matchedChi2 );
      Track* matchedTrack = new Track();
      /// Copy T track
      matchedTrack->copy( *(*t) );
      /// Add muon ids to copied T track
      BOOST_FOREACH( LHCbID id, matchedMuonTrack->lhcbIDs() ) matchedTrack->addToLhcbIDs( id );
      matchedTracks->insert( matchedTrack );
    }
  }
  
  Tuple tuple = nTuple( 100, "OT-Muon Matching N-Tuple" );
  tuple->column( "nTTracks"   , tTracks->size() );
  tuple->column( "nMuonTracks", mTracks->size() );
  tuple->column( "nMatches"   , nMatches        );
      
  tuple->farray( "matchChi2s", matchChi2s, "num", 100 );

  tuple->farray( "matchTx"    ,  bind<double>(&State::x    , _1),
                 "matchTxErr2",  bind<double>(&State::errX2, _1),
                 "matchTy"    ,  bind<double>(&State::y    , _1),
                 "matchTyErr2",  bind<double>(&State::errY2, _1),
                 m_matchTStates.begin(), m_matchTStates.end(),
                 "num",
                 100 );
  tuple->farray( "matchTtx"    , bind<double>(&State::tx    , _1),
                 "matchTtxErr2", bind<double>(&State::errTx2, _1),
                 "matchTty"    , bind<double>(&State::ty    , _1),
                 "matchTtyErr2", bind<double>(&State::errTy2, _1),
                 m_matchTStates.begin(), m_matchTStates.end(),
                 "num",
                 100 );
  tuple->farray( "matchMx"    ,  bind<double>(&State::x    , _1),
                 "matchMxErr2",  bind<double>(&State::errX2, _1),
                 "matchMy"    ,  bind<double>(&State::y    , _1),
                 "matchMyErr2",  bind<double>(&State::errY2, _1),
                 m_matchMuonStates.begin(), m_matchMuonStates.end(),
                 "num",
                 100 );
  tuple->farray( "matchMtx"    , bind<double>(&State::tx    , _1),
                 "matchMtxErr2", bind<double>(&State::errTx2, _1),
                 "matchMty"    , bind<double>(&State::ty,     _1),
                 "matchMtyErr2", bind<double>(&State::errTy2, _1),
                 m_matchMuonStates.begin(), m_matchMuonStates.end(),
                 "num",
                 100 );
  tuple->write();
  
  //if ( fullDetail() ) plot( goodMatches, 103, "Number of good mathces", -0.5, 10.5, 11 );

  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::matchChi2( const LHCb::Track& tTrack, const Track& mTrack, 
                                             const double& atZ,
                                             double& chi2 ) {
  StatusCode sc = StatusCode::SUCCESS;

  /// Get the T state closest to this z
  State tState = tTrack.closestState( atZ );
  /// Get the Muon state closest to this z
  State mState = mTrack.closestState( atZ );
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "Closest T state to z = "    << atZ << " is " << tState << endmsg
            << "Closest Muon state to z = " << atZ << " is " << mState << endmsg;
  }

  /// Now extrapolate these states
  sc = m_extrapolator->propagate( tState, atZ );
  if ( !sc.isSuccess() ) Warning( "Could not propagate T state"   , StatusCode::FAILURE, 5 );
  sc = m_extrapolator->propagate( mState, atZ );
  if ( !sc.isSuccess() ) Warning( "Could not propagate Muon state", StatusCode::FAILURE, 5 );
  
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "Extrapolated T state to z = "    << atZ << " is " << tState << endmsg
            << "Extrapolated Muon state to z = " << atZ << " is " << mState << endmsg;
  }

  /// Now calculate the match chi2
  sc = m_chi2Calculator->calculateChi2( tState.stateVector(), tState.covariance(), 
                                        mState.stateVector(), mState.covariance(),
                                        chi2 );
  if ( !sc.isSuccess() ) Error( "Could not invert matrices", StatusCode::FAILURE );

  if ( chi2 > 0.0 && chi2 < m_matchChi2Cut ) {
    m_matchTStates.push_back( tState );
    m_matchMuonStates.push_back( mState );
  }
  
  //   m_matchTx.push_back(     tState.x()                   );
  //   m_matchTxErr.push_back(  std::sqrt( tState.errX2    ) );
  //   m_matchTtx.push_back(    tState.tx()                  );
  //   m_matchTtxErr.push_back( std::sqrt( tState.errTx2() ) );
  //   m_matchTy.push_back(     tState.y()                   );
  //   m_matchTyErr.push_back(  std::sqrt( tState.errY2    ) );
  //   m_matchTty.push_back(    tState.ty()                  );
  //   m_matchTtyErr.push_back( std::sqrt( tState.errTy2() ) );

  //   m_matchMx.push_back(     mState.x()                   );
  //   m_matchMxErr.push_back(  std::sqrt( mState.errX2() )  );
  //   m_matchMtx.push_back(    mState.tx()                  );
  //   m_matchMtxErr.push_back( std::sqrt( mState.errTx2() ) );
  //   m_matchMy.push_back(     mState.y()                   );
  //   m_matchMyErr.push_back(  std::sqrt( mState.errY2() )  );
  //   m_matchMty.push_back(    mState.ty()                  );
  //   m_matchMtyErr.push_back( std::sqrt( mState.errTy2() ) );

  // if ( fullDetail() && chi2 < m_matchChi2Cut ) {
  //     plot( tState.x()  - mState.x() , 200, "Match Delta x" , -400, 400, 200  );
  //     plot( tState.tx() - mState.tx(), 201, "Match Delta tx", -0.3, 0.3, 100  );
  //     if ( tState.tx() < 0.0 )  plot( tState.x()  - mState.x() , 202, "Match Delta x for tx < 0" , -400, 400, 200  );
  //     if ( tState.tx() > 0.0 )  plot( tState.x()  - mState.x() , 203, "Match Delta x for tx > 0" , -400, 400, 200  );
  
  //     plot( tState.y()  - mState.y() , 300, "Match Delta y" , -800, 800, 200 );
  //     plot( tState.ty() - mState.ty(), 301, "Match Delta ty", -0.3, 0.3, 100  );
  //     if ( tState.ty() < 0.0 )  plot( tState.y()  - mState.y() , 302, "Match Delta y for ty < 0" , -800, 800, 200 );
  //     if ( tState.ty() > 0.0 )  plot( tState.y()  - mState.y() , 303, "Match Delta y for ty > 0" , -800, 800, 200 );
  //   }

  return sc;
}
