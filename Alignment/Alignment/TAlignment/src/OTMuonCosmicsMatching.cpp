// $Id: OTMuonCosmicsMatching.cpp,v 1.1 2008-10-31 20:57:53 janos Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

// from LHCbMath
#include "LHCbMath/MatrixTransforms.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/Track.h"
#include "Event/State.h"

// from MuonNNet
#include "MuonNNet/MuonTrack.h"

// local
#include "OTMuonCosmicsMatching.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTMuonCosmicsMatching
//
// 2008-10-28 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMuonCosmicsMatching );

OTMuonCosmicsMatching::OTMuonCosmicsMatching( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "TracksToMatchLocation" , m_tracksToMatchLoc = TrackLocation::Default    );
  declareProperty( "Extrapolator"          , m_nameExtrapolator = "TrackMasterExtrapolator" );
  declareProperty( "MatchAtZ"              , m_matchAtZ         = 15000*Gaudi::Units::mm    );
}

OTMuonCosmicsMatching::~OTMuonCosmicsMatching() {} 

StatusCode OTMuonCosmicsMatching::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_muonRecTool  = tool<IMuonNNetRec>( "MuonNNetRec" );
  m_extrapolator = tool<ITrackExtrapolator>( m_nameExtrapolator, "Extrapolator",  this );
   
  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  typedef std::vector<MuonTrack*>  MuonTracks;

  const MuonTracks* muonTracks    = m_muonRecTool->tracks();
  const Tracks*     tracksToMatch = get<Tracks>( m_tracksToMatchLoc );
  
  if ( fullDetail() ) {
    plot1D( muonTracks->size()   , 100, "Number of Muon Tracks"    , -0.5, 10.5, 11 );
    plot1D( tracksToMatch->size(), 101, "Number of Tracks to Match", -0.5, 10.5, 11 );
  }
  
  unsigned goodMatches = 0;

  for ( Tracks::const_iterator t = tracksToMatch->begin(), tEnd = tracksToMatch->end(); t != tEnd; ++t ) {
    for ( MuonTracks::const_iterator m = muonTracks->begin(), mEnd = muonTracks->end(); m != mEnd; ++m ) {
      double chi2 = matchChi2( *(*t), *(*m), m_matchAtZ );
      if ( fullDetail() ) {
        plot( chi2, 102, "Match chi2", 0, 100, 100 );
        if ( chi2 < 20.0 ) ++goodMatches;
      }
    }
  }
 
  if ( fullDetail() ) plot( goodMatches, 103, "Number of good mathces", -0.5, 10.5, 11 );
    
  return StatusCode::SUCCESS;
}

StatusCode OTMuonCosmicsMatching::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

StatusCode OTMuonCosmicsMatching::extrapolate( MyState& state, double oldZ, double newZ ) const {
  
  double dZ = newZ - oldZ;
  if ( std::abs( dZ ) < TrackParameters::propagationTolerance ) {
    if ( msgLevel( MSG::DEBUG  ) ) debug() << "Already at required z position" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  /// x' = T*x
  Gaudi::Matrix4x4 jack = jacobian( dZ );
  state.first = jack*state.first;
  /// C' = JCJ^T. Cov is symmetric
  state.second  = ROOT::Math::Similarity( jack, state.second );
  
  return StatusCode::SUCCESS;
}

double OTMuonCosmicsMatching::matchChi2( const LHCb::Track& match, const MuonTrack& to, double atZ ) const {
  /// First the "LHCb" track
  /// Get the state we want to extrapolate from
  State lhcbState = match.closestState( atZ );
  if ( msgLevel( MSG::DEBUG  ) ) debug() << "Closest state is " << lhcbState << endmsg;
  
  MyState   state = std::make_pair( lhcbState.stateVector().Sub<Gaudi::Vector4>( 0 ),
                                    lhcbState.covariance().Sub<Gaudi::SymMatrix4x4>( 0, 0 ) );
  /// Now let's extrapolate to atZ
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "My T state is " << state.first << endmsg
            << " with covatiance " << state.second << endmsg
            << "Going to extrapolate from " << state.first << " to " << atZ << endmsg;
  }
  //StatusCode sc = m_extrapolator->propagate( lhcbState, atZ );
  StatusCode   sc = extrapolate( state, lhcbState.z(), atZ );
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "My New T state is " << state.first << endmsg
            << " with covariance " << state.second << endmsg;
  }

  /// Now the "Muon" track
  /// Get the point we want to extrapolate to
  /// I assume this is in global coordinates. Looks like it.
  /// Need to cast away constness. No const getHits method.
  MuonTrack&       mTrack = const_cast<MuonTrack&>(to);
  /// Get the last muon hit that the track was fitted with
  Gaudi::XYZPoint   point = *(mTrack.getHits().back());
  //Gaudi::Vector4   mState( mTrack.bx(), mTrack.by(), mTrack.sx(), mTrack.sy() );
  Gaudi::SymMatrix4x4 mCov; // Symmetric. At "z = 0"
  mCov( 0, 0 ) = mTrack.errbx()*mTrack.errbx();
  mCov( 0, 2 ) = mTrack.covbsx();
  mCov( 1, 1 ) = mTrack.errby()*mTrack.errby();
  mCov( 1, 3 ) = mTrack.covbsy();
  mCov( 2, 2 ) = mTrack.errsx()*mTrack.errsx();
  mCov( 3, 3 ) = mTrack.errsy()*mTrack.errsy();
  MyState mState = std::make_pair( Gaudi::Vector4( point.z()*mTrack.sx() + mTrack.bx(),
                                                   point.z()*mTrack.sy() + mTrack.by(), 
                                                   mTrack.sx(), 
                                                   mTrack.sy() ),
                                   ROOT::Math::Similarity( jacobian( point.z() ),  mCov ) );
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "Muon hit (x,y,z) : " << point.x() << ", " << point.y() << ", " << point.z() << endmsg
            << "My muon track state is " << mState.first << endmsg
            << " with covariance " << mState.second << endmsg
            << "Going to extrapolate from " << mState.first << " to " << atZ << endmsg;
  }
  /// Now extrapolate muon track
  sc = extrapolate( mState, point.z(), atZ );
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "My New Muon state is " << mState.first << endmsg
            << " with covariance " << mState.second << endmsg;
  }

  /// X^2 = ( x_OT - x_Muon )( C_OT + C_Muon )^-1( x_OT - x_Muon )^T
  int iFail = 0; 
  const Gaudi::Vector4      dif  = mState.first - state.first;
  const Gaudi::SymMatrix4x4 sumC = mState.second + state.second;
  const double chi2 = ROOT::Math::Similarity( dif, sumC.Inverse( iFail ) );
  /// Shouldn't fail
  if ( 0 != iFail ) std::cout << "Oops something went wrong" << std::endl;
  if ( msgLevel( MSG::DEBUG  ) ) debug() << "Match chi2 is " << chi2 << endmsg;
  return chi2;
}
