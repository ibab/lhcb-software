
// local
#include "Event/PackedTrack.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

//-----------------------------------------------------------------------------

void TrackPacker::pack( const DataVector & tracks,
                        PackedDataVector & ptracks ) const
{
  ptracks.tracks().reserve(tracks.size());

  for ( DataVector::const_iterator itT = tracks.begin();
        tracks.end() != itT; ++itT )
  {
    const LHCb::Track* track = *itT;
    if ( !track ) continue;

    ptracks.tracks().push_back( LHCb::PackedTrack() );
    LHCb::PackedTrack & newTrk = ptracks.tracks().back();

    newTrk.key        = track->key();
    newTrk.chi2PerDoF = m_pack.fltPacked( track->chi2PerDoF() );
    newTrk.nDoF       = track->nDoF();
    newTrk.flags      = track->flags();
    newTrk.likelihood = m_pack.fltPacked( track->likelihood() );
    newTrk.ghostProba = m_pack.fltPacked( track->ghostProbability() );

    //== Store the LHCbID as int
    newTrk.firstId    = ptracks.ids().size();
    for ( std::vector<LHCb::LHCbID>::const_iterator itI = track->lhcbIDs().begin();
          track->lhcbIDs().end() != itI; ++itI )
    {
      ptracks.ids().push_back( (*itI).lhcbID() );
    }
    newTrk.lastId    = ptracks.ids().size();
    if( UNLIKELY( parent().msgLevel(MSG::DEBUG) ) )
      parent().debug() << "Stored LHCbIDs from "
                       << newTrk.firstId << " to " << newTrk.lastId << endmsg;

    //== Handle the states in the track
    newTrk.firstState = ptracks.states().size();
    for ( std::vector<LHCb::State*>::const_iterator itS = track->states().begin();
          track->states().end() != itS; ++itS )
    {
      convertState( **itS, ptracks );
    }
    newTrk.lastState = ptracks.states().size();
    if( UNLIKELY( parent().msgLevel(MSG::DEBUG) ) )
      parent().debug() << "Stored states from " << newTrk.firstState
                       << " to " << newTrk.lastState << endmsg;

    //== Handles the ExtraInfo
    newTrk.firstExtra = ptracks.extras().size();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = track->extraInfo().begin();
          track->extraInfo().end() != itE; ++itE )
    {
      ptracks.extras().push_back( std::pair<int,int>( (*itE).first,
                                                      m_pack.fltPacked((*itE).second) ) );
    }
    newTrk.lastExtra = ptracks.extras().size();

  }
}

void TrackPacker::convertState( const LHCb::State& state,
                                PackedDataVector & ptracks ) const
{
  ptracks.states().push_back( LHCb::PackedState() );
  LHCb::PackedState & newState = ptracks.states().back();

  newState.flags = state.flags();

  newState.x    = m_pack.position( state.x()  );
  newState.y    = m_pack.position( state.y()  );
  newState.z    = m_pack.position( state.z()  );
  newState.tx   = m_pack.slope   ( state.tx() );
  newState.ty   = m_pack.slope   ( state.ty() );
  double p = 0.;
  if ( 0 != state.qOverP() ) p = 1./ state.qOverP();
  newState.p    = m_pack.energy  ( p );
  //== Get the coded value in case of saturation, to code properly the error later
  p = m_pack.energy( newState.p );

  // convariance Matrix
  std::vector<double> err;
  err.push_back( std::sqrt( state.errX2() ) );
  err.push_back( std::sqrt( state.errY2() ) );
  err.push_back( std::sqrt( state.errTx2() ) );
  err.push_back( std::sqrt( state.errTy2() ) );
  err.push_back( std::sqrt( state.errQOverP2() ) );

  newState.cov_00 = m_pack.position( err[0] );
  newState.cov_11 = m_pack.position( err[1] );
  newState.cov_22 = m_pack.slope   ( err[2] );
  newState.cov_33 = m_pack.slope   ( err[3] );
  newState.cov_44 = m_pack.energy  ( 1.e5 * fabs(p) * err[4] ); //== 1.e5 * dp/p (*1.e2)

  newState.cov_10 = m_pack.fraction( state.covariance()(1,0)/err[1]/err[0] );
  newState.cov_20 = m_pack.fraction( state.covariance()(2,0)/err[2]/err[0] );
  newState.cov_21 = m_pack.fraction( state.covariance()(2,1)/err[2]/err[1] );
  newState.cov_30 = m_pack.fraction( state.covariance()(3,0)/err[3]/err[0] );
  newState.cov_31 = m_pack.fraction( state.covariance()(3,1)/err[3]/err[1] );
  newState.cov_32 = m_pack.fraction( state.covariance()(3,2)/err[3]/err[2] );
  newState.cov_40 = m_pack.fraction( state.covariance()(4,0)/err[4]/err[0] );
  newState.cov_41 = m_pack.fraction( state.covariance()(4,1)/err[4]/err[1] );
  newState.cov_42 = m_pack.fraction( state.covariance()(4,2)/err[4]/err[2] );
  newState.cov_43 = m_pack.fraction( state.covariance()(4,3)/err[4]/err[3] );
}

void TrackPacker::unpack( const PackedDataVector & ptracks,
                          DataVector             & tracks ) const
{
  tracks.reserve( ptracks.tracks().size() );

  int firstIdHigh    = 0;
  int lastIdHigh     = 0;
  int firstStateHigh = 0;
  int lastStateHigh  = 0;
  int firstExtraHigh = 0;
  int lastExtraHigh  = 0;

  for ( std::vector<PackedTrack>::const_iterator itS = ptracks.tracks().begin();
        ptracks.tracks().end() != itS; ++itS )
  {
    const LHCb::PackedTrack& src = (*itS);

    LHCb::Track* track = new LHCb::Track( );
    tracks.insert( track, src.key );

    track->setChi2PerDoF( m_pack.fltPacked( src.chi2PerDoF ) );
    track->setNDoF(       src.nDoF );
    track->setFlags(      src.flags );
    if ( ptracks.version() > 2 )
    {
      track->setLikelihood(       m_pack.fltPacked( src.likelihood ) );
      track->setGhostProbability( m_pack.fltPacked( src.ghostProba ) );
    }

    int firstId = src.firstId;
    int lastId  = src.lastId;
    if ( ptracks.ids().size() > 65535 )
    {
      //== Protections for wraping of the index, a short unsigned int.
      firstId = firstIdHigh + src.firstId;
      lastId  = lastIdHigh  + src.lastId;
      if ( lastId < firstId )
      { // we wrapped in the track
        parent().info() << "** ID index wrapped, first/last Id = "
                        << firstId << " " << lastId << endmsg;
        lastIdHigh  += 0x10000;
        firstIdHigh += 0x10000;
        lastId = lastIdHigh  + src.lastId;
      }
    }

    std::vector<LHCb::LHCbID> lhcbids( lastId - firstId ) ;
    std::vector<LHCb::LHCbID>::iterator lhcbit = lhcbids.begin() ;
    for ( int kId = firstId; lastId > kId; ++kId, ++lhcbit )
    {
      const unsigned int& id = *(ptracks.ids().begin()+kId);
      *lhcbit = LHCb::LHCbID( id ) ;
    }
    // schema change: sorting no longer needed when we write DSTs with sorted lhcbids
    if ( ptracks.version() <= 1 )
    {
      std::sort( lhcbids.begin(), lhcbids.end() ) ;
    }
    track->addSortedToLhcbIDs( lhcbids ) ;

    int firstState = src.firstState;
    int lastState  = src.lastState;
    if ( ptracks.states().size() > 65535 )
    {
      //== Protections for wraping of the index, a short unsigned int.
      firstState = firstStateHigh + src.firstState;
      lastState  = lastStateHigh  + src.lastState;
      if ( lastState < firstState )
      {  // we wrapped in the track
        parent().info() << "** State index wrapped, first/last = "
                        << firstState << " " << lastState << endmsg;
        lastStateHigh  += 0x10000;
        firstStateHigh += 0x10000;
        lastState = lastStateHigh  + src.lastState;
      }
    }

    for ( int kSt = firstState; lastState > kSt; ++kSt )
    {
      const LHCb::PackedState& pSta = *(ptracks.states().begin()+kSt);
      convertState( pSta, *track );
    }

    int firstExtra = src.firstExtra;
    int lastExtra  = src.lastExtra;
    if ( ptracks.extras().size() > 65535 )
    {
      //== Protections for wraping of the index, a short unsigned int.
      firstExtra = firstExtraHigh + src.firstExtra;
      lastExtra  = lastExtraHigh  + src.lastExtra;
      if ( lastExtra < firstExtra )
      { // we wrapped in the track
        parent().info() << "** Extra index wrapped, first/last = "
                        << firstExtra << " " << lastExtra << endmsg;
        lastExtraHigh  += 0x10000;
        firstExtraHigh += 0x10000;
        lastExtra = lastExtraHigh  + src.lastExtra;
      }
    }

    for ( int kEx = firstExtra; lastExtra > kEx; ++kEx )
    {
      const std::pair<int,int>& info = *(ptracks.extras().begin()+kEx);
      track->addInfo( info.first, m_pack.fltPacked( info.second ) );
    }

    //== Cleanup extraInfo and set likelihood/ghostProbability for old data
    if ( ptracks.version() <= 2 )
    {
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
      track->setLikelihood(       track->info(   1, 999) ); // was the key of likelihood...
      track->setGhostProbability( track->info( 102, 999) ); // was the key of ghost probability
      track->eraseInfo(   1 );
      track->eraseInfo( 102 );
    }

  }

}

void TrackPacker::convertState ( const LHCb::PackedState& pSta,
                                 LHCb::Track & tra ) const
{
  LHCb::StateVector stateVector;
  stateVector.setX( m_pack.position( pSta.x ) );
  stateVector.setY( m_pack.position( pSta.y ) );
  stateVector.setZ( m_pack.position( pSta.z ) );
  stateVector.setTx( m_pack.slope( pSta.tx ) );
  stateVector.setTy( m_pack.slope( pSta.ty ) );
  const double pInv = ( 0 != pSta.p ? 1.0/m_pack.energy(pSta.p) : 0.0 );
  stateVector.setQOverP( pInv );

  LHCb::State state( stateVector );

  //== Fill covariance matrix
  const double err0 = m_pack.position( pSta.cov_00 );
  const double err1 = m_pack.position( pSta.cov_11 );
  const double err2 = m_pack.slope   ( pSta.cov_22 );
  const double err3 = m_pack.slope   ( pSta.cov_33 );
  const double err4 = m_pack.energy  ( pSta.cov_44 ) * fabs(pInv) * 1.e-5;
  Gaudi::TrackSymMatrix & stateCov = *(const_cast<Gaudi::TrackSymMatrix*>(&state.covariance()));
  stateCov(0,0) = err0 * err0;
  stateCov(1,1) = err1 * err1;
  stateCov(2,2) = err2 * err2;
  stateCov(3,3) = err3 * err3;
  stateCov(4,4) = err4 * err4;
  stateCov(1,0) = err1 * err0 * m_pack.fraction( pSta.cov_10 );
  stateCov(2,0) = err2 * err0 * m_pack.fraction( pSta.cov_20 );
  stateCov(2,1) = err2 * err1 * m_pack.fraction( pSta.cov_21 );
  stateCov(3,0) = err3 * err0 * m_pack.fraction( pSta.cov_30 );
  stateCov(3,1) = err3 * err1 * m_pack.fraction( pSta.cov_31 );
  stateCov(3,2) = err3 * err2 * m_pack.fraction( pSta.cov_32 );
  stateCov(4,0) = err4 * err0 * m_pack.fraction( pSta.cov_40 );
  stateCov(4,1) = err4 * err1 * m_pack.fraction( pSta.cov_41 );
  stateCov(4,2) = err4 * err2 * m_pack.fraction( pSta.cov_42 );
  stateCov(4,3) = err4 * err3 * m_pack.fraction( pSta.cov_43 );

  state.setFlags( pSta.flags );

  tra.addToStates( state );
}

StatusCode TrackPacker::check( const DataVector & dataA,
                               const DataVector & dataB ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( dataA.version() != dataB.version() )
  {
    return parent().Error( "Version number mis-match" );
  }

  if ( dataA.size() != dataB.size() )
  {
    parent().err() << "Old Track size " << dataA.size()
                   << " differs form Test " << dataB.size()
                   << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::Tracks::const_iterator itOld  = dataA.begin();
  LHCb::Tracks::const_iterator itTest = dataB.begin();
  while ( dataA.end() != itOld )
  {
    LHCb::Track* oTrack = (*itOld++);
    LHCb::Track* tTrack = (*itTest++);
    if ( oTrack->key() != tTrack->key() )
    {
      parent().warning() << "Wrong key : old " <<  oTrack->key() << " test " << tTrack->key() << endmsg;
    }
    bool isOK = true;
    if ( 1.e-7 < fabs( (oTrack->chi2PerDoF()-tTrack->chi2PerDoF())/tTrack->chi2PerDoF() ) ) isOK = false;
    if ( 0   < abs( oTrack->nDoF() - tTrack->nDoF() ) ) isOK = false;
    if ( oTrack->flags() != tTrack->flags() )              isOK = false;
    if ( oTrack->lhcbIDs().size() != tTrack->lhcbIDs().size() ) isOK = false;
    if ( 1.e-7 < fabs( (oTrack->likelihood() - tTrack->likelihood() )/ tTrack->likelihood() ) ) isOK = false;
    if ( 1.e-7 < fabs( (oTrack->ghostProbability()-tTrack->ghostProbability())/
                       tTrack->ghostProbability()) ) isOK = false;
    unsigned int kk;
    for ( kk = 0 ; oTrack->lhcbIDs().size() != kk ; ++kk )
    {
      if ( oTrack->lhcbIDs()[kk].lhcbID() != tTrack->lhcbIDs()[kk].lhcbID() )     isOK = false;
    }
    const LHCb::Track::ExtraInfo& oExtra = oTrack->extraInfo();
    const LHCb::Track::ExtraInfo& tExtra = tTrack->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::Track::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::Track::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt )
    {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }

    if ( oTrack->nStates() != tTrack->nStates() ) isOK = false;

    if ( !isOK || MSG::DEBUG >= parent().msgLevel() )
    {
      parent().info() << "===== Track key " << oTrack->key() << endmsg;
      parent().info() << format( "Old   chi2 %10.4f  nDoF %6i flags %8x nLhcbID %4d nExtra %4d  nStates %4d",
                                 oTrack->chi2PerDoF(), oTrack->nDoF(), oTrack->flags(),
                                 oTrack->lhcbIDs().size(), oExtra.size(), oTrack->nStates() );
      parent().info() << format( " Likelihood %10.6f ghostProba %10.8f",
                                 oTrack->likelihood(), oTrack->ghostProbability() )
                      << endmsg;
      parent().info() << format( "Test  chi2 %10.4f  nDoF %6i flags %8x nLhcbID %4d nExtra %4d  nStates %4d",
                                 tTrack->chi2PerDoF(), tTrack->nDoF(), tTrack->flags(),
                                 tTrack->lhcbIDs().size(), tExtra.size(), tTrack->nStates() );
      parent().info() << format( " Likelihood %10.6f ghostProba %10.8f",
                                 tTrack->likelihood(), tTrack->ghostProbability() )
                      << endmsg;
      for ( kk = 0 ; oTrack->lhcbIDs().size() != kk ; ++kk ) {
        parent().info() << format( "   old ID %8x   new %8x",
                                   oTrack->lhcbIDs()[kk].lhcbID(),
                                   tTrack->lhcbIDs()[kk].lhcbID() ) << endmsg;
      }
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) {
        parent().info() << format( "   old Extra %5d %12.4f     new %5d %12.4f",
                                   (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second )
                        << endmsg;
      }
    }

    //== Compare the states. The track number won't be reported...

    for ( kk = 0; (oTrack->nStates() > kk) && (tTrack->nStates() > kk); ++kk )
    {
      compareStates( *oTrack->states()[kk], *tTrack->states()[kk] );
    }
  }

  return sc;
}

void TrackPacker::compareStates ( const LHCb::State& oSta,
                                  const LHCb::State& tSta ) const
{
  bool isOK = true;

  if ( 5.e-5 < fabs( oSta.z() - tSta.z() ) ) isOK = false;
  if ( 5.e-5 < fabs( oSta.x() - tSta.x() ) ) isOK = false;
  if ( 5.e-5 < fabs( oSta.y() - tSta.y() ) ) isOK = false;
  if ( 5.e-8 < fabs( oSta.tx() - tSta.tx() ) ) isOK = false;
  if ( 5.e-8 < fabs( oSta.ty() - tSta.ty() ) ) isOK = false;
  double oP = 1./oSta.qOverP();
  double tP = 1./tSta.qOverP();
  if ( 5.e-3 < fabs( oP - tP ) ) isOK = false;

  std::vector<double> oDiag;
  oDiag.push_back( std::sqrt(oSta.errX2()) );
  oDiag.push_back( std::sqrt(oSta.errY2()) );
  oDiag.push_back( std::sqrt(oSta.errTx2()) );
  oDiag.push_back( std::sqrt(oSta.errTy2()) );
  oDiag.push_back( std::sqrt(oSta.errQOverP2() ) );

  std::vector<double> tDiag;
  tDiag.push_back( std::sqrt(tSta.errX2()) );
  tDiag.push_back( std::sqrt(tSta.errY2()) );
  tDiag.push_back( std::sqrt(tSta.errTx2()) );
  tDiag.push_back( std::sqrt(tSta.errTy2()) );
  tDiag.push_back( std::sqrt(tSta.errQOverP2() ) );

  if ( 5.e-5 < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
  if ( 5.e-5 < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
  if ( 5.e-8 < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
  if ( 5.e-8 < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;
  //== Don't report problem if the term saturated: 2.e9 times energy scale 1.e-2
  if ( 5.    < fabs( oDiag[4]*oP*1.e5 - tDiag[4]*tP*1.e5 ) &&
       fabs( tDiag[4]*tP*1.e5 ) < 1.999e7                   ) isOK = false;

  std::vector<double> oFrac;
  oFrac.push_back( oSta.covariance()(1,0) / oDiag[1] / oDiag[0] );
  oFrac.push_back( oSta.covariance()(2,0) / oDiag[2] / oDiag[0] );
  oFrac.push_back( oSta.covariance()(2,1) / oDiag[2] / oDiag[1] );
  oFrac.push_back( oSta.covariance()(3,0) / oDiag[3] / oDiag[0] );
  oFrac.push_back( oSta.covariance()(3,1) / oDiag[3] / oDiag[1] );
  oFrac.push_back( oSta.covariance()(3,2) / oDiag[3] / oDiag[2] );
  oFrac.push_back( oSta.covariance()(4,0) / oDiag[4] / oDiag[0] );
  oFrac.push_back( oSta.covariance()(4,1) / oDiag[4] / oDiag[1] );
  oFrac.push_back( oSta.covariance()(4,2) / oDiag[4] / oDiag[2] );
  oFrac.push_back( oSta.covariance()(4,3) / oDiag[4] / oDiag[3] );

  std::vector<double> tFrac;
  tFrac.push_back( tSta.covariance()(1,0) / tDiag[1] / tDiag[0] );
  tFrac.push_back( tSta.covariance()(2,0) / tDiag[2] / tDiag[0] );
  tFrac.push_back( tSta.covariance()(2,1) / tDiag[2] / tDiag[1] );
  tFrac.push_back( tSta.covariance()(3,0) / tDiag[3] / tDiag[0] );
  tFrac.push_back( tSta.covariance()(3,1) / tDiag[3] / tDiag[1] );
  tFrac.push_back( tSta.covariance()(3,2) / tDiag[3] / tDiag[2] );
  tFrac.push_back( tSta.covariance()(4,0) / tDiag[4] / tDiag[0] );
  tFrac.push_back( tSta.covariance()(4,1) / tDiag[4] / tDiag[1] );
  tFrac.push_back( tSta.covariance()(4,2) / tDiag[4] / tDiag[2] );
  tFrac.push_back( tSta.covariance()(4,3) / tDiag[4] / tDiag[3] );

  unsigned int kk;
  for ( kk = 0 ; oFrac.size() > kk ; ++kk ) 
  {
    if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
  }

  if ( MSG::VERBOSE >= parent().msgLevel() ) isOK = false; //== force printing
  if ( !isOK ) 
  {
    parent().info() << "=== State differ: " << endmsg;
    parent().info() << "     old "
                    << format( " %12.5f %12.5f %12.5f %12.9f %12.9f %12.3f",
                               oSta.z(), oSta.x(), oSta.y(),
                               oSta.tx(), oSta.ty(), 1./oSta.qOverP() )
                    << endmsg;
    parent().info() << "    test "
                    << format( " %12.5f %12.5f %12.5f %12.9f %12.9f %12.3f",
                               tSta.z(), tSta.x(), tSta.y(),
                               tSta.tx(), tSta.ty(), 1./tSta.qOverP() )
                    << endmsg;
    parent().info() << format( " old Diag %10.5f %10.5f %12.9f %12.9f %12.3f",
                               oDiag[0], oDiag[1], oDiag[2], oDiag[3], oDiag[4]*oP*1.e5 )
                    << endmsg;
    parent().info() << format( "test Diag %10.5f %10.5f %12.9f %12.9f %12.3f",
                               tDiag[0], tDiag[1], tDiag[2], tDiag[3], tDiag[4]*tP*1.e5 )
                    << endmsg;
    parent().info() << " old Frac ";
    for ( kk = 0 ; oFrac.size() > kk ; ++kk )
    {
      parent().info() << format( " %8.5f", oFrac[kk] );
    }
    parent().info() << endmsg << "test Frac ";
    for ( kk = 0 ; tFrac.size() > kk ; ++kk )
    {
      parent().info() << format( " %8.5f", tFrac[kk] );
    }
    parent().info() << endmsg;
  }

}
