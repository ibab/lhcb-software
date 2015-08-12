// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"
#include "TrackKernel/StateTraj.h"

// track tools
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"

// local
#include "LongTrackReferenceCreator.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( LongTrackReferenceCreator )

//=============================================================================
// 
//=============================================================================
LongTrackReferenceCreator::LongTrackReferenceCreator(const std::string& type,
					     const std::string& name,
					     const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructer
  declareInterface<ITrackManipulator>(this);
}

//=============================================================================
// 
//=============================================================================
LongTrackReferenceCreator::~LongTrackReferenceCreator(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode LongTrackReferenceCreator::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  // extrapolator
  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// 
//=============================================================================

template<class T>
inline bool LessThanFirst(const T& lhs, const T& rhs)
{
  return lhs.first < rhs.first ;
}

StatusCode LongTrackReferenceCreator::execute( LHCb::Track& track ) const
{
  // given existing states on the track, this tool adds states at fixed
  // z-positions along the track. if a track state already exists
  // sufficiently close to the desired state, it will not add the
  // state.
  StatusCode sc = StatusCode::SUCCESS ;

  // first fix the momentum of states on the track. need to make sure this works for Velo-TT as well.
  if( track.states().empty() ) {
    error() << "Track has no states. Cannot create reference states" << endmsg ;
    sc = StatusCode::FAILURE ;
  } else {
    // first need to make sure all states already on track have
    // reasonable momentum. still needs to check that this works for
    // velo-TT
    const LHCb::State* stateAtT = track.stateAt(LHCb::State::AtT) ;
    const LHCb::State& refstate = stateAtT ? *stateAtT :
      *( track.checkFlag(Track::Backward) ? track.states().front() : track.states().back()) ;
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
          StatusCode thissc = m_extrapolator->propagate( statevec, jt->first ) ;
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
