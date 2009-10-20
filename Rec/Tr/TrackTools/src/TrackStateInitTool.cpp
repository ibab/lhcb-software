#include "TrackStateInitTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackStateInitTool
//
// 2009-03-02 : Pavel Krokovny
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackStateInitTool ) ;

TrackStateInitTool::TrackStateInitTool( const std::string& type,
					const std::string& name,
					const IInterface* parent)
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackStateInit>(this);
}

TrackStateInitTool::~TrackStateInitTool() {}

StatusCode TrackStateInitTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  m_seedFit = tool<IPatSeedFit>( "PatSeedFit" );
  m_veloFitter = tool<ITrackFitter>("Tf::PatVeloFitLHCbIDs", "FitVelo", this) ;
  m_veloTTFit = tool<IPatVeloTTFit>("PatVeloTTFit");
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator", 
					    "Extrapolator",this);
  return sc ;
}

StatusCode TrackStateInitTool::fit( LHCb::Track& track, bool clearStates ) const
{



  
  
  //do nothing  
  if(!clearStates) 
    return StatusCode::SUCCESS;

  // cleanup track entirely
  LHCb::Track::Types savedType = track.type();

  unsigned int flags = track.flags();

  track.clearStates() ;
  track.setFitResult(0) ;

  // create velo states
  if(savedType <= LHCb::Track::Upstream){

      LHCb::Track::ExtraInfo extraInfo = track.extraInfo();

      const SmartRefVector<LHCb::Track> ancestors = track.ancestors();      

      StatusCode sc = createVeloStates( track );
      if( sc.isFailure() ) Warning("TrackStateInitool fit Velo failed",sc,0).ignore();

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
  if(savedType == LHCb::Track::Upstream) {
      StatusCode sc = createVeloTTStates( track );
      if( sc.isFailure() ) Warning("TrackStateInitTool fit TT failed",sc,0).ignore();
  }

  // set the T-station states
  if(savedType == LHCb::Track::Long ||
     savedType == LHCb::Track::Downstream ||
     savedType == LHCb::Track::Ttrack){
      StatusCode sc = createTStationStates( track ) ;
      if( sc.isFailure() ) Warning("TrackStateInitTool fit T failed",sc,0).ignore();
  }

  return StatusCode::SUCCESS ;
} ;

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
  StatusCode success = m_seedFit->fitSeed(track.lhcbIDs(), &newStates);
  //set qOverP for Velo states
  if(newStates.size()>0 && track.nStates()>0 && 
     track.firstState().location()<=LHCb::State::EndVelo) {
    double qOverP = newStates.begin()->qOverP();
    double errQOverP = newStates.begin()->errQOverP2();
    for( LHCb::Track::StateContainer::const_iterator istate = track.states().begin();
	 istate != track.states().end(); ++istate) {
      if((*istate)->location() <= LHCb::State::EndVelo) {
	(const_cast<LHCb::State*>(*istate))->setQOverP( qOverP ) ;
	(const_cast<LHCb::State*>(*istate))->setErrQOverP2(errQOverP) ;
      }
      //create TT state
      if((*istate)->location() == LHCb::State::EndVelo) {
	LHCb::StateVector statevec( (*istate)->stateVector(), (*istate)->z() ) ;
	double zpos = StateParameters::ZEndTT ;
	StatusCode thissc = m_extrapolator->propagate( statevec, zpos, 0, 
						       LHCb::ParticleID(211)); 
	LHCb::State newState = LHCb::State( statevec ) ;
	newState.setLocation(LHCb::State::LocationUnknown);
	track.addToStates( newState ) ;
      }
    }
  }
  
  for( std::vector<LHCb::State>::iterator istate = newStates.begin();
       istate != newStates.end(); ++istate) {
    track.addToStates(*istate);
  }
  return success ;
}

StatusCode TrackStateInitTool::initializeRefStates(LHCb::Track& track,
						   LHCb::ParticleID pid ) const
{
  // given existing states on the track, this tool adds states at fixed
  // z-positions along the track. if a track state already exists
  // sufficiently close to the desired state, it will not add the
  // state.
  StatusCode sc = StatusCode::SUCCESS ;

  // first fix the momentum of states on the track. need to make sure this works for Velo-TT as well.
  if( track.states().empty() ) {
    sc = Error( "Track has no state! Can not fit.", StatusCode::FAILURE );
  } else {
    // first need to make sure all states already on track have
    // reasonable momentum. still needs to check that this works for
    // velo-TT
    
    const LHCb::State& refstate = track.hasStateAt(LHCb::State::AtT) ? *(track.stateAt(LHCb::State::AtT)) :
      ( track.checkFlag(LHCb::Track::Backward) ? *(track.states().front()) : *(track.states().back())) ;
    

    for( LHCb::Track::StateContainer::const_iterator it = track.states().begin() ;
         it != track.states().end() ; ++it)
      const_cast<LHCb::State*>(*it)->setQOverP( refstate.qOverP() ) ;
    
    
    // collect the z-positions where we want the states
    std::vector<double> zpositions ;
    if( track.hasT() ) {
      zpositions.push_back( StateParameters::ZBegT) ;
      zpositions.push_back( StateParameters::ZEndT ) ;
    }
    if( track.hasTT() || (track.hasT() && track.hasVelo() ) ) 
      zpositions.push_back(StateParameters::ZEndTT) ;
    if( track.hasVelo() )
      zpositions.push_back(StateParameters::ZEndVelo) ;
    
    // the following container is going to hold pairs of 'desired'
    // z-positionds and actual states. the reason for the gymnastics
    // is that we always want to propagate from the closest availlable
    // state, but then recursively. this will make the parabolic
    // approximation reasonably accurate.
    typedef std::pair<double, const LHCb::State*> ZPosWithState ;
    typedef std::vector< ZPosWithState > ZPosWithStateContainer ;
    std::vector< ZPosWithState > states ;
    // we first add the states we already have
    for( std::vector<LHCb::State*>::const_iterator it = track.states().begin() ;
         it != track.states().end() ; ++it) 
      states.push_back( ZPosWithState((*it)->z(),(*it)) ) ;

    // now add the other z-positions, provided nothing close exists
    const double maxDistance = 50*Gaudi::Units::cm ;
    for( std::vector<double>::iterator iz = zpositions.begin() ;
         iz != zpositions.end(); ++iz) {
      bool found = false ;
      for( ZPosWithStateContainer::const_iterator it = states.begin() ;
           it != states.end()&&!found ; ++it)
        found = fabs( *iz - it->first ) < maxDistance ;
      if(!found) states.push_back( ZPosWithState(*iz,0) ) ;
    }
    std::sort( states.begin(), states.end(), LessThanFirst<ZPosWithState> ) ;
    
    // create the states in between
    LHCb::Track::StateContainer newstates ;
    for( ZPosWithStateContainer::iterator it = states.begin();
         it != states.end() ; ++it) 
      if( it->second == 0 ) {
        // find the nearest existing state to it
        ZPosWithStateContainer::iterator best= states.end() ;
        for( ZPosWithStateContainer::iterator jt = states.begin();
             jt != states.end() ; ++jt) 
          if( it != jt && jt->second
              && ( best==states.end() || fabs( jt->first - it->first) < fabs( best->first - it->first) ) )
            best = jt ;
        
        assert( best != states.end() ) ;
        
        // move from that state to this iterator, using the extrapolator and filling all states in between.
        int direction = best > it ? -1 : +1 ;
        LHCb::StateVector statevec( best->second->stateVector(), best->second->z() ) ;
        for( ZPosWithStateContainer::iterator jt = best+direction ;
             jt != it+direction ; jt += direction) {
          StatusCode thissc = m_extrapolator->propagate( statevec, jt->first, 0, pid ) ;
          LHCb::State* newstate = new LHCb::State( statevec ) ;
          jt->second = newstate ;
          newstates.push_back( newstate ) ;
          if( !thissc.isSuccess() ) {
            error() << "Problem propagating state in LongTrackReferenceCreator::createReferenceStates" << endmsg ;
            sc = thissc ;
          }
        }
      }
    
    // finally, copy the new states to the track. 
    track.addToStates( newstates ) ;
  }
  return sc ;
}
