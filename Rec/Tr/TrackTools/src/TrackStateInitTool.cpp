#include "TrackStateInitTool.h"
#include "STDet/DeSTDetector.h"

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
  ,m_veloFitter(0)
  ,m_veloFitterName("")
{
  declareInterface<ITrackStateInit>(this);
  declareProperty( "VeloFitterName" , m_veloFitterName = "Tf::PatVeloFitLHCbIDs/FitVelo");
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
  return sc ;
}

StatusCode TrackStateInitTool::fit( LHCb::Track& track, bool clearStates ) const
{
  StatusCode sc = StatusCode::SUCCESS ;

  //do nothing  
  if(!clearStates) return sc ;

  // save type and flags since patrec fit may reset those
  LHCb::Track::Types savedType = track.type();
  unsigned int flags = track.flags();
  
  // save the first state such that we have something if procedure fails
  LHCb::State saveState = track.nStates()>0 ? track.firstState() : LHCb::State() ;

  // now clean up all fit history
  track.clearStates() ;
  track.setFitResult(0) ;

  // create velo states
  if(savedType <= LHCb::Track::Upstream){

    // why do we need to cache extraInfo and ancestors?
      LHCb::Track::ExtraInfo extraInfo = track.extraInfo();

      const SmartRefVector<LHCb::Track> ancestors = track.ancestors();      

      sc = createVeloStates( track );
      if( sc.isFailure() ) Warning("TrackStateInitTool fit Velo failed",sc,0).ignore();
      

      track.clearAncestors();
      // copy the ancestors
      for (SmartRefVector<LHCb::Track>::const_iterator it = ancestors.begin();
         it != ancestors.end();  ++it) track.addToAncestors(*(*it));
    
      track.setExtraInfo(extraInfo); 
  }
   
  track.setPatRecStatus( LHCb::Track::PatRecIDs );
  track.setFitStatus( LHCb::Track::FitStatusUnknown );
  track.setType(savedType);
  track.setFlags(flags);

  // set momentum for veloTT tracks
  if(sc.isSuccess() && savedType == LHCb::Track::Upstream) {
      sc = createVeloTTStates( track );
      if( sc.isFailure() ) Warning("TrackStateInitTool fit TT failed",sc,0).ignore();
  }

  // set the T-station states
  if(sc.isSuccess() && (savedType == LHCb::Track::Long ||
			 savedType == LHCb::Track::Downstream ||
			 savedType == LHCb::Track::Ttrack) ) {
    sc = createTStationStates( track ) ;
    if( sc.isFailure() ) Warning("TrackStateInitTool fit T failed",sc,0).ignore();
    else {
      if( /* savedType == LHCb::Track::Long || */
	 savedType == LHCb::Track::Downstream ) {
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
  // let's just run velo fitter
  return m_veloFitter->fit(track) ;
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
    double qOverP = newStates.begin()->qOverP();
    double errQOverP = newStates.begin()->errQOverP2();
      
    // replace the first T state by the extrapolated Velo state
    const LHCb::State* velottstate = track.states().back() ;
    LHCb::StateVector statevec( velottstate->stateVector(), velottstate->z() ) ;
    statevec.setQOverP( qOverP ) ;
    LHCb::State& tstate = newStates.front() ;
    sc = m_extrapolator->propagate( statevec, tstate.z(), 0 );
    tstate.stateVector() = statevec.parameters() ;
    
    BOOST_FOREACH( LHCb::State* state, track.states() ) {
      state->setQOverP( qOverP ) ;
      state->setErrQOverP2(errQOverP) ;
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
  if( refstate==0 ) refstate = &(track.firstState()) ;
  
  const double zref=StateParameters::ZEndTT ;
  LHCb::StateVector statevec( refstate->stateVector(), refstate->z() ) ;
  Gaudi::TrackMatrix jacobian ;
  StatusCode sc = m_extrapolator->propagate( statevec, zref, &jacobian );

  if( sc.isSuccess() ) {
    // collect the TT hits
    std::vector< LHCb::LHCbID > ttids ;
    std::set<int> ttlayers ;
    BOOST_FOREACH( const LHCb::LHCbID& id, track.lhcbIDs() ) 
      if( id.isTT() ) {
	ttids.push_back( id ) ;
	ttlayers.insert( id.stID().layer() ) ;
      }
    
    // we'll only try something more complicated if there are enough TT hits
    if( ttlayers.size() >= 3 ) {
      
      //      double xtt(0) ;
      typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> LineHit ;
      std::vector<LineHit> tthits ;
      BOOST_FOREACH( const LHCb::LHCbID& id, ttids ) {
        LHCb::STChannelID ttid = id.stID() ;
        const DeSTSector* sector = m_ttdetector->findSector(ttid) ;
        double dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend ;
        sector->trajectory( ttid.strip(), 0, dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend) ;
        tthits.push_back( LineHit( LineHit::Point(xAtYEq0,0,zAtYEq0),LineHit::Vector(dxdy,1,dzdy) ) ) ;
        //	if( !sector->isStereo() ) xtt = xAtYEq0 ;
      }
      
      Gaudi::Vector4 ttstatevec = statevec.parameters().Sub<Gaudi::Vector4>(0) ;
      //ttstatevec(0) = xtt ;
      const bool fixTy = true ;
      fitLine( zref, ttstatevec, tthits, fixTy ) ;
      for(size_t i=0; i<4; ++i) statevec.parameters()(i) = ttstatevec(i) ;
      
      // I have also tried to fix q/p but it simply doesn't help anything.
      
      // also fix ty: the ty of the seed track fit is actually quite poor.
      double ty = 
	(track.states().front()->y() - ttstatevec(1))/
	(track.states().front()->z() - zref)  ;
      
      statevec.setTy ( ty ) ;
      track.states().front()->setTy(  statevec.ty() ) ;
    }
    
    // now add the TT state
    LHCb::State ttstate(statevec) ;
    ttstate.setLocation( LHCb::State::AtTT ) ;
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

