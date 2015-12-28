#include "TrackStateInitTool.h"
#include "STDet/DeSTDetector.h"
#include "Kernel/HitPattern.h"
#include "TrackInterfaces/ITrackMomentumEstimate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackStateInitTool
//
// 2009-03-02 : Pavel Krokovny
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackStateInitTool )

TrackStateInitTool::TrackStateInitTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
: GaudiTool ( type, name , parent )
  ,m_veloFitterName("")
  ,m_veloFitter(0)
  ,m_fastMomentumTool(0)
{
  declareInterface<ITrackStateInit>(this);
  declareProperty( "VeloFitterName" , m_veloFitterName = "Tf::PatVeloFitLHCbIDs/FitVelo");
  declareProperty( "ptVelo", m_ptVelo = 400.*Gaudi::Units::MeV);
  declareProperty( "UseFastMomentumEstimate", m_useFastMomentumEstimate = false);
  declareProperty( "FastMomentumToolName", m_fastMomentumToolName = "FastMomentumEstimate");
}

TrackStateInitTool::~TrackStateInitTool() {}

StatusCode TrackStateInitTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  m_seedFit = tool<IPatSeedFit>( "PatSeedFit" );
  m_veloFitter = tool<ITrackFitter>(m_veloFitterName, this) ;
  m_veloTTFit = tool<IPatVeloTTFit>("PatVeloTTFit");
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",
                                            "Extrapolator",this);
  m_ttdetector = getDet<DeSTDetector>(DeSTDetLocation::location("TT")) ;
  if (m_useFastMomentumEstimate)
    m_fastMomentumTool = tool<ITrackMomentumEstimate>(m_fastMomentumToolName);
  else
    m_fastMomentumTool = nullptr;
  return sc ;
}

StatusCode TrackStateInitTool::fit( LHCb::Track& track, bool clearStates ) const
{
  StatusCode sc = StatusCode::SUCCESS ;

  //do nothing
  if(!clearStates) return sc ;

  // save the first state such that we have something if procedure fails
  LHCb::State saveState = track.nStates()>0 ? track.firstState() : LHCb::State() ;

  // now clean up all fit history
  track.clearStates() ;
  track.setFitResult(0) ;
  track.setFitStatus( LHCb::Track::FitStatusUnknown );

  // create velo states
  LHCb::HitPattern hitpattern( track.lhcbIDs() ) ;

  if(  hitpattern.numVeloStations() > 0 ) {
    // at states in velo
    sc = createVeloStates( track );
    if( sc.isFailure() ) Warning("TrackStateInitTool fit Velo failed",sc,0).ignore();

    // add state in TT, but not for long tracks since the fit is actually rather poor
    if(sc.isSuccess() && hitpattern.numTTHits()>0 && hitpattern.numTLayers()==0 ) {
      sc = createVeloTTStates( track );
      if( sc.isFailure() ) Warning("TrackStateInitTool fit TT failed",sc,0).ignore();
    }
  }

  // set the T-station states
  if(sc.isSuccess() && hitpattern.numTLayers()>0) {
    // add states in T
    sc = createTStationStates( track ) ;
    if( sc.isFailure() ) Warning("TrackStateInitTool fit T failed",sc,0).ignore();
    else {
      // add TT state for downstream tracks
      if( hitpattern.numTTHits()>0 &&  hitpattern.numVeloStations()==0 ) {
        sc = createTTState( track ) ;
        if( sc.isFailure() ) Warning("TrackStateInitTool fit TT failed",sc,0).ignore();
      }
    }
  }

  // if something really failed, make sure to copy the first state back
  if( track.nStates() == 0 ) {
    track.addToStates( saveState ) ;
    if( sc.isSuccess() ) sc = StatusCode::FAILURE ;
  }

  return sc ;
}

StatusCode TrackStateInitTool::createVeloStates( LHCb::Track& track ) const
{
  // need to cache some things that velofitter removes
  LHCb::Track::Types savedType = track.type();
  unsigned int flags = track.flags();
  LHCb::Track::ExtraInfo extraInfo = track.extraInfo();
  const SmartRefVector<LHCb::Track> ancestors = track.ancestors();

  StatusCode sc = m_veloFitter->fit(track) ;

  track.clearAncestors();
  for (SmartRefVector<LHCb::Track>::const_iterator it = ancestors.begin();
       it != ancestors.end();  ++it) track.addToAncestors(*(*it));
  track.setExtraInfo(extraInfo);
  track.setFlags(flags) ;
  track.setType(savedType) ;

  // assign 'standard' momentum. from TrackPrepareVelo
  if( m_ptVelo > 0 ) {
    int charge = 2*(track.key()%2)-1 ;
    double tx = track.firstState().tx() ;
    double ty = track.firstState().ty() ;
    double slope2 = std::max( tx*tx + ty*ty, 1e-20);
    double qop = charge * sqrt( slope2 ) / (m_ptVelo * sqrt( 1. + slope2 ));
    for( LHCb::State* state: track.states() ) {
      state->setQOverP( qop ) ;
    }
  }

  return sc ;
}

StatusCode TrackStateInitTool::createVeloTTStates( LHCb::Track& track) const
{
  return m_veloTTFit->fitVTT(track);
}


StatusCode TrackStateInitTool::createTStationStates( LHCb::Track& track ) const
{

  std::vector<LHCb::State> newStates;
  //set T states
  StatusCode sc = m_seedFit->fitSeed(track.lhcbIDs(), &newStates);
  //set qOverP for Velo states
  if(sc.isSuccess() &&
     newStates.size()>0 && track.nStates()>0 &&
     track.firstState().location()<=LHCb::State::EndVelo) {
    double qOverP = newStates.front().qOverP();
    double errQOverP = newStates.front().errQOverP2();

    // I want to get a better estimate for q/p. This is done by
    // propgating from A to B, and then adapt qop to get delta-x =0
    // However, the result seems rather senstive to where you start
    // from. Things I tried:
    // - extrapolate velo-TT state to T1. this works very poorly,
    //   probably because the slope in TT is so bad.
    // - extrapolate velo-end state to T1 and match in X. this works
    //   better, but is still 2x worse than the pT kick.
    // - extrapolate T1 to velo. still a it worse than pT kick.
    // - extrapolate T2 to velo-end. this works better than the pT
    //   kick, but it is a bit biased.
    // - extrapolate T2 to velo-TT.  this works about as good as just
    //   the pT kick. (strange ... what's wrong with TT?!)
    // - extrapolate velo to T2. this works still a bit worse than the
    //   pT kick. (strange ... is velo slope worse than T slope?)

    // Maybe this pattern can partially be explained by how material
    // is distributed. In that case, it should get better if you match
    // slopes. So, that's what I tried next:
    // - extrapolate velo-TT to T2. this still works very poorly. the
    //   TT state is just so bad that I think I need to remove it again.
    // - extrapolate velo to T2. this now works about as good as T2 to
    //   velo matching x. reso a bit worse, but less outliers. bias
    //   the same.
    // - extrapolate T2 to velo. that gives about the same result
    //

    // So, in the end I have chosen to propagate the TMid state to
    // velo and match in X. That's also close to what we do for
    // Downstream.

    const LHCb::State* velostate = track.stateAt( LHCb::State::EndVelo ) ;
    const LHCb::State& t2state = newStates[1] ; // this is middle of T

    if (m_fastMomentumTool){
      sc = m_fastMomentumTool->calculate(velostate, &t2state,
                                         qOverP, errQOverP, true);
      // Factor of 2.5 to scale the error up for the Kalman fit.
      // However, it does not seem to have any effect.
      errQOverP = errQOverP*errQOverP*2.5;
    }else{
      LHCb::StateVector statevec( t2state.stateVector(), t2state.z() ) ;
      Gaudi::TrackMatrix jacobian ;
      sc = m_extrapolator->propagate( statevec, velostate->z(), &jacobian );
      if( jacobian(0,4)!=0 ) {
        double dqop = (velostate->x() - statevec.x()) / jacobian(0,4) ;
        qOverP += dqop ;
      }

      // using this momentum, we can now improve the T1 state. it
      // doesn't make any real difference, even w/o updating transport.
      statevec = LHCb::StateVector( t2state.stateVector(), t2state.z() ) ;
      statevec.setQOverP( qOverP ) ;
      LHCb::State& t1state = newStates[0] ;
      sc = m_extrapolator->propagate( statevec, t1state.z(), 0);
      t1state.stateVector() = statevec.parameters() ;
    }

    // now update q/p of all other states
    for( LHCb::State* state: track.states() ) {
      state->setQOverP( qOverP ) ;
      state->setErrQOverP2(errQOverP) ;
    }
    for( std::vector<LHCb::State>::iterator istate = newStates.begin();
         istate != newStates.end(); ++istate){
      istate->setQOverP( qOverP ) ;
      istate->setErrQOverP2( errQOverP ) ;
    }

  }

  for( std::vector<LHCb::State>::iterator istate = newStates.begin();
       istate != newStates.end(); ++istate) {
    track.addToStates(*istate);
  }
  return sc ;
}


namespace {
  template<class HitContainer>
  void fitLine( double z, Gaudi::Vector4& statevector,
                const HitContainer& hits, bool fixTy)
  {
    Gaudi::Vector4 dChi2dX, H ;
    Gaudi::SymMatrix4x4 d2Chi2dX2 ;

    Gaudi::XYZVector d1(statevector(2),statevector(3),1) ;
    Gaudi::XYZVector p1(statevector(0),statevector(1),z) ;

    for( typename HitContainer::const_iterator ihit = hits.begin() ;
         ihit != hits.end() ; ++ ihit) {
      typename HitContainer::value_type::Vector d2 = ihit->direction() ;
      typename HitContainer::value_type::Point  p2 = ihit->beginPoint() ;
      typename HitContainer::value_type::Vector deltap(p1.x()-p2.x(),p1.y()-p2.y(),p1.z()-p2.z()) ;
      typename HitContainer::value_type::Vector nvec = d1.Cross( d2 ) ;
      double n = nvec.R() ;
      double d = deltap.Dot( nvec) / n ;
      double dndtx = 1/n * (d2.y() * nvec.z() - d2.z() * nvec.y() ) ;
      double dndty = 1/n * (d2.z() * nvec.x() - d2.x() * nvec.z() ) ;
      double dnydtx = - d2.z() ;
      double dnzdtx =   d2.y() ;
      double dddtx = 1/n * ( dnydtx * deltap.y() + dnzdtx * deltap.z() ) - d/(n*n) * dndtx ;
      double dnxdty =   d2.z() ;
      double dnzdty = - d2.x() ;
      double dddty = 1/n * ( dnxdty * deltap.x() + dnzdty * deltap.z() ) - d/(n*n) * dndty ;
      double dddx  = nvec.x()/n ;
      double dddy  = nvec.y()/n ;

      H(0) = dddx ;
      H(1) = dddy ;
      H(2) = dddtx ;
      H(3) = dddty ;

      dChi2dX += d * H ;
      for(size_t irow=0; irow<4; ++irow)
        for(size_t icol=0; icol<=irow; ++icol)
          d2Chi2dX2(irow,icol) += H(irow) * H(icol) ;
    }

    if( fixTy ) {
      Gaudi::SymMatrix3x3 d2Chi2dX2Sub = d2Chi2dX2.Sub<Gaudi::SymMatrix3x3>(0,0) ;
      d2Chi2dX2Sub.InvertChol() ;
      for(size_t irow=0; irow<3; ++irow)
        for(size_t icol=0; icol<3; ++icol)
          statevector(irow) -= d2Chi2dX2Sub(irow,icol) * dChi2dX(icol) ;
    } else {
      d2Chi2dX2.InvertChol() ;
      statevector -= d2Chi2dX2 * dChi2dX ;
    }
  }
}

StatusCode TrackStateInitTool::createTTState(LHCb::Track& track ) const
{
  // this routine fits the TT hits to model with only 3 parameters: x, y and tx

  // first get a reference state: if there are velo hits, take the
  // state end velo, otherwise take a T state
  LHCb::State* refstate = track.stateAt( LHCb::State::EndVelo ) ;
  if( refstate==0 ) refstate = &(track.closestState(StateParameters::ZMidT )) ;
  if( refstate==0 ) refstate = &(track.firstState()) ;

  const double zref=StateParameters::ZMidTT ;
  LHCb::StateVector statevec( refstate->stateVector(), refstate->z() ) ;
  Gaudi::TrackMatrix jacobian ;
  StatusCode sc = m_extrapolator->propagate( statevec, zref, &jacobian );

  if( sc.isSuccess() ) {
    // collect the TT hits
    std::vector< LHCb::LHCbID > ttids ;
    std::set<int> ttlayers ;
    for( const LHCb::LHCbID& id: track.lhcbIDs() )
      if( id.isTT() ) {
        ttids.push_back( id ) ;
        LHCb::STChannelID stid = id.stID() ;
        unsigned int uniquelayer = (stid.station()-1)*2 + stid.layer()-1 ;
        ttlayers.insert( uniquelayer ) ;
      }

    // we'll only try something more complicated if there are enough TT hits
    if( ttlayers.size() >= 3 ) {

      //      double xtt(0) ;
      typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> LineHit ;
      std::vector<LineHit> tthits ;
      for( const LHCb::LHCbID& id: ttids ) {
        LHCb::STChannelID ttid = id.stID() ;
        const DeSTSector* sector = m_ttdetector->findSector(ttid) ;
        double dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend ;
        sector->trajectory( ttid.strip(), 0, dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend) ;
        tthits.push_back( LineHit( LineHit::Point(xAtYEq0,0,zAtYEq0),LineHit::Vector(dxdy,1,dzdy) ) ) ;
        // if( !sector->isStereo() ) xtt = xAtYEq0 ;
      }

      Gaudi::Vector4 ttstatevec = statevec.parameters().Sub<Gaudi::Vector4>(0) ;
      //ttstatevec(0) = xtt ;
      const bool fixTy = true ;
      fitLine( zref, ttstatevec, tthits, fixTy ) ;
      double dqop = jacobian(0,4) != 0 ? (ttstatevec(0) - statevec.x()) / jacobian(0,4) : 0 ;
      for(size_t i=0; i<4; ++i) statevec.parameters()(i) = ttstatevec(i) ;

      if( track.type() == LHCb::Track::Downstream ) {
        // also fix ty: the ty of the seed track fit is actually quite poor.
        double ty =
          (track.states().front()->y() - ttstatevec(1))/
          (track.states().front()->z() - zref)  ;
        statevec.setTy ( ty ) ;

        // set the ty only in T1. should we also set tx? that requires
        // another extrapolation.
        track.states().front()->setTy( ty ) ;
      }

      // set the qop of all states
      if( track.type() != LHCb::Track::Long ) {
        double qop = statevec.qOverP() + dqop ;
        statevec.setQOverP( qop ) ;
        for( LHCb::State* state: track.states() )
          state->setQOverP( qop ) ;
      }
    }

    // now add the TT state
    LHCb::State ttstate(statevec) ;
    ttstate.setLocation( LHCb::State::AtTT ) ; // use AtTT for both TT
    ttstate.setCovariance( refstate->covariance() ) ;
    track.addToStates( ttstate ) ;
  }

  return sc ;
}


StatusCode TrackStateInitTool::initializeRefStates(LHCb::Track& ,
                                                   LHCb::ParticleID) const
{
  error() << "TrackStateInitTool::initializeRefStates. Code was copied from TrackMasterFitter. Let's leave it there."
          << endmsg ;
  return StatusCode::FAILURE ;
}

