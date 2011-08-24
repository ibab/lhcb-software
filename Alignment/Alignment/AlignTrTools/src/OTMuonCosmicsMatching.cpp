// $Id: OTMuonCosmicsMatching.cpp,v 1.4 2009-07-15 15:11:24 wouter Exp $
// Include files 

// from STD
#include <algorithm>
#include <iterator>

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

namespace MatchingHelpers {
 struct MatchedTrack {
   Track*                tTrack      ;
   unsigned              multiplicity;
   std::vector< Track* > muonTracks  ;
   std::vector< double > matchChi2s  ;
 };
 
 struct PositiveLessThan {
   bool operator()( const double& chi2One, const double chi2Two ) {
     return ( chi2One < chi2Two && chi2One > 0.0 );
   };
 };
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMuonCosmicsMatching );

OTMuonCosmicsMatching::OTMuonCosmicsMatching( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "TTracksLocation"          , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "MuonTracksLocation"       , m_mTracksLocation      = TrackLocation::Muon        );
  declareProperty( "TracksOutputLocation"     , m_tracksOutputLocation = m_tTracksLocation+"/TMuon" );
  declareProperty( "Extrapolator"             , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "Chi2Calculator"           , m_nChi2Calculator      = "TrackChi2Calculator"      );
  declareProperty( "MatchAtZ"                 , m_matchAtZ             = 12500*Gaudi::Units::mm     );
  declareProperty( "MatchAtFirstMuonHit"      , m_matchAtFirstMuonHit  = false                      );
  declareProperty( "MatchChi2Cut"             , m_matchChi2Cut         = 20.0                       );
  declareProperty( "AllCombinations"          , m_allCombinations      = true                       );
  declareProperty( "AddMuonIDsToMatchedTTrack", m_addMuonIDs           = true                       );
}

OTMuonCosmicsMatching::~OTMuonCosmicsMatching() {} 

StatusCode OTMuonCosmicsMatching::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_extrapolator   = tool<ITrackExtrapolator>(   m_nExtrapolator  , "Extrapolator"  , this );
  m_chi2Calculator = tool<ITrackChi2Calculator>( m_nChi2Calculator, "Chi2Calculator", this );
  m_muonDet        = getDet<DeMuonDetector>( DeMuonLocation::Default );
  
  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::execute() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  const Tracks* tTracks = get<Tracks>( m_tTracksLocation );
  const Tracks* mTracks = get<Tracks>( m_mTracksLocation );
  Tracks*        output = new Tracks();
  put( output, m_tracksOutputLocation );

  std::vector< MatchingHelpers::MatchedTrack > matchedTracks;
  matchedTracks.reserve( tTracks->size() );
  
  for ( Tracks::const_iterator t = tTracks->begin(), tEnd = tTracks->end(); t != tEnd; ++t ) {
    MatchingHelpers::MatchedTrack tmTrack;
    tmTrack.tTrack       = (*t);
    tmTrack.multiplicity =    0;
    tmTrack.muonTracks.resize( mTracks->size(), 0       );
    tmTrack.matchChi2s.resize( mTracks->size(), -9999.0 );
    /// Now match this T track to muon tracks
    for ( Tracks::const_iterator m = mTracks->begin(), mEnd = mTracks->end(); m != mEnd; ++m ) {
      double chi2   = -9999.0;
      double    z   = m_matchAtZ;
      State* mState = 0;
      if ( m_matchAtFirstMuonHit ) {
        if ( (*m)->hasStateAt( State::Muon ) )  
          mState = ( (*m)->stateAt( State::Muon ) );
        else if ( (*m)->hasStateAt( State::FirstMeasurement )  ) 
          mState = ( (*m)->stateAt( State::FirstMeasurement ) );
        else   
          Warning( "Couldn't find muon state. Going to extrapolate to default z", StatusCode::FAILURE, 5 ).ignore();
        z = mState->z();
        debug() << "Found muon state. Going to extrapolate to this state with z = " << z << endmsg;
      }
      /// Get the T state closest to this z
      State* tState = &( (*t)->closestState( z ) );
      /// Get the Muon state closest to this z
      mState        = &( (*m)->closestState( z ) );
      /// Calculate match chi2
      StatusCode   sc = matchChi2( *tState, *mState, z, chi2 );
      if ( sc.isSuccess() && chi2 > -1.0 ) {
        ++tmTrack.multiplicity;
        tmTrack.muonTracks[ std::distance( mTracks->begin(), m ) ] = (*m);
        tmTrack.matchChi2s[ std::distance( mTracks->begin(), m ) ] = chi2;
      }
    }
    if ( tmTrack.multiplicity > 0 ) matchedTracks.push_back( tmTrack );
  }

  filter( matchedTracks, *output );

  if ( fullDetail() ) fillNTuple( matchedTracks );
  
  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::matchChi2( LHCb::State& tState, LHCb::State& mState, 
                                             const double& atZ,
                                             double& chi2 ) {
  StatusCode sc = StatusCode::SUCCESS;

  /// Extrapolate states
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

  return sc;
}

void OTMuonCosmicsMatching::filter( const std::vector< MatchingHelpers::MatchedTrack >& matchedTracks, 
                                    LHCb::Tracks& output ) const {
  for ( std::vector< MatchingHelpers::MatchedTrack >::const_iterator m = matchedTracks.begin();
        m != matchedTracks.end(); ++m ) {
    if ( m_allCombinations ) {
      for ( std::vector< double >::const_iterator chi2 = (*m).matchChi2s.begin();
            chi2 != (*m).matchChi2s.end() ; ++chi2 ) {
        if ( (*chi2) > 0.0 && (*chi2) < m_matchChi2Cut ) {
          Track* track = new Track();
          merge( *(*m).tTrack,
                 *( (*m).muonTracks[ std::distance( (*m).matchChi2s.begin(), chi2 ) ] ), 
                 *track );
          output.insert( track );
        }
      }
    } else { /// Take only the best
      std::vector<double>::const_iterator chi2 = min_element( (*m).matchChi2s.begin(), 
                                                              (*m).matchChi2s.end()  ,
                                                              MatchingHelpers::PositiveLessThan() );
      if ( (*chi2) < m_matchChi2Cut ) {
        Track* track = new Track();
        merge( *(*m).tTrack, 
               *( (*m).muonTracks[ std::distance( (*m).matchChi2s.begin(), chi2 ) ] ), 
               *track );
        output.insert( track );
      }
    }
  }
}

void OTMuonCosmicsMatching::merge( const LHCb::Track& tTrack, const LHCb::Track& muonTrack, LHCb::Track& track ) const {
  track.copy( tTrack );
  if ( m_addMuonIDs ) {
    BOOST_FOREACH( LHCbID id, muonTrack.lhcbIDs() ) {
      track.addToLhcbIDs( id );
    }
  }
}

void OTMuonCosmicsMatching::fillNTuple( const std::vector< MatchingHelpers::MatchedTrack >& matchedTracks ) const {

  Tuple ntuple = nTuple( 100, "OT-Muon Matching N-Tuple" );

  for ( std::vector< MatchingHelpers::MatchedTrack >::const_iterator t = matchedTracks.begin();
        t != matchedTracks.end(); ++t ) {
    std::vector< double > chi2s;
    for ( std::vector< Track* >::const_iterator m = (*t).muonTracks.begin(); m != (*t).muonTracks.end(); ++m ) {
      if ( (*m) ) {
        double chi2 = (*t).matchChi2s[ std::distance( (*t).muonTracks.begin(), m ) ];
        std::vector< State  > tStates;
        std::vector< int    > station, region, chamber;
        std::vector< double > tileX, tileY, tileZ;
        std::vector< double > tileDx2, tileDy2, tileDz2;
        /// Get muon ids
        const std::vector< LHCbID >& ids = (*m)->lhcbIDs();
        for ( std::vector< LHCbID >::const_iterator id = ids.begin(), idEnd = ids.end(); id != idEnd; ++id ) {
          if ( id->isMuon() ) {
            MuonTileID tileID = id->muonID();
            double x, dx, y, dy, z, dz;
            /// Get tile xyz in global coordinates
            StatusCode sc = m_muonDet->Tile2XYZ( tileID, x, dx, y, dy, z, dz );
            if ( sc.isSuccess() ) {
              debug() << "************* Tile info *************"
                      << " Tile = " << tileID << endreq
                      << "    x = " <<    x << "  y = " <<  y << "  z = " <<  z << endreq
                      << "   dx = " <<   dx << " dy = " << dy << " dz = " << dz << endreq
                      << " station = " << tileID.station() << " region " << tileID.region() << endreq
                      << "*************************************" << endreq;
              /// Extrapolate T state to this z
              State state = (*t).tTrack->closestState( z );
              sc = m_extrapolator->propagate( state, z );
              if ( sc.isSuccess() ) {
                /// OK fill ntuple info
                tStates.push_back( state );
                station.push_back( tileID.station() );
                region.push_back( tileID.region() );
                std::vector< DeMuonChamber* > chambers = m_muonDet->Tile2Chamber( tileID );
                debug() << " Got " << chambers.size() << " muon chambers." << endmsg;
                chamber.push_back( chambers.front()->chamberNumber() );
                tileX.push_back( x );
                tileDx2.push_back( dx*dx );
                tileY.push_back( y );
                tileDy2.push_back( dy*dy );
                tileZ.push_back( z );
                tileDy2.push_back( dz*dz );
              } else {
                Warning( "Failed to extrapolate T State" , StatusCode::FAILURE, 0 ).ignore();
              }
            }
          }
        }
        ntuple->column( "multiTTrack", (*t).multiplicity );
        ntuple->column( "matchChi2"  , chi2 );
        ntuple->farray( "mStations" ,  station.begin(), station.end(), "num", 100 );
        ntuple->farray( "mRegions"  ,   region.begin(),  region.end(), "num", 100 );
        ntuple->farray( "mChambers" ,  chamber.begin(), chamber.end(), "num", 100 );
        ntuple->farray( "tileX"   ,     tileX.begin(),    tileX.end(), "num", 100 );
        ntuple->farray( "tileY"   ,     tileY.begin(),    tileY.end(), "num", 100 );
        ntuple->farray( "tileZ"   ,     tileZ.begin(),    tileZ.end(), "num", 100 );
        ntuple->farray( "tileDx2" ,   tileDx2.begin(),  tileDx2.end(), "num", 100 );
        ntuple->farray( "tileDy2" ,   tileDy2.begin(),  tileDy2.end(), "num", 100 );
        ntuple->farray( "tileDz2" ,   tileDz2.begin(),  tileDz2.end(), "num", 100 );
        
        ntuple->farray( "tStateX" ,  bind<double>(&State::x    , _1),
                        "tStateXerr2",  bind<double>(&State::errX2, _1),
                        "tStateY"    ,  bind<double>(&State::y    , _1),
                        "tStateYerr2",  bind<double>(&State::errY2, _1),
                        tStates.begin(), tStates.end(),
                        "num",
                        100 );
        ntuple->farray( "tStateTx"    , bind<double>(&State::tx    , _1),
                        "tStateTxErr2", bind<double>(&State::errTx2, _1),
                        "tStateTy"    , bind<double>(&State::ty    , _1),
                        "tStateTyErr2", bind<double>(&State::errTy2, _1),
                        tStates.begin(), tStates.end(),
                        "num",
                        100 );
        ntuple->farray( "tStateZ"     , bind<double>( &State::z, _1 ),
                        tStates.begin(), tStates.end(),
                        "num",
                        100 );
        ntuple->write(); /// For each matched track
      }
    }
  }
}

