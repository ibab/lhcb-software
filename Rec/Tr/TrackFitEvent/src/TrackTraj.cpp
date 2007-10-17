
#include "Event/TrackTraj.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include <algorithm>

namespace LHCb
{
  
  inline bool compareStateZ(const State* lhs, const State* rhs)
  {
    return lhs->z() < rhs->z() ;
  }

  inline bool equalStateZ(const State* lhs, const State* rhs)
  {
    return fabs(lhs->z() - rhs->z()) < TrackParameters::propagationTolerance ;
  }
  
  TrackTraj::TrackTraj(const Track& track, const IMagneticFieldSvc* magfieldsvc)
    : Trajectory(0,0), m_bfield(0,0,0)
  {
    // the sorting takes a lot of time. therefore, we rely on the fact
    // that nodes and states in a track are already sorted. we use
    // 'std::merge' to merge the sorted ranges.
    
    // first add the states from the track.nodes(). make sure these
    // are ordered in increasing z.
    TrackTraj::StateContainer statesfromnodes ;
    statesfromnodes.reserve( track.nodes().size() ) ;
    if(!track.nodes().empty()) {
      if( track.nodes().front()->z() < track.nodes().back()->z() ) {
	//nodes in right order
	for(std::vector<Node*>::const_iterator it = track.nodes().begin() ;
	    it != track.nodes().end(); ++it) 
	  m_states.push_back( &((*it)->state() )) ;
      } else {
	// nodes in wrong order
	for(std::vector<Node*>::const_reverse_iterator it = track.nodes().rbegin() ;
	    it != track.nodes().rend(); ++it) 
	  m_states.push_back( &((*it)->state() )) ;
      }
    }
    
    // now use std::merge to add track.states(). we could also use
    // inplace_merge: then we don't need to create a vector for the
    // nodes first. I don't know what is faster.
    m_states.resize( statesfromnodes.size() + track.states().size() ) ;
    std::merge( statesfromnodes.begin(), statesfromnodes.end(),
		track.states().begin(), track.states().end(),
		m_states.begin(), compareStateZ ) ;
    
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  TrackTraj::TrackTraj(const TrackTraj::StateContainer& states, const IMagneticFieldSvc* magfieldsvc)
    : Trajectory(0,0),m_states(states), m_bfield(0,0,0)
  {
    // sort
    std::sort(m_states.begin(), m_states.end(),compareStateZ) ;
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  void TrackTraj::init(const IMagneticFieldSvc* magfieldsvc)
  {
    // add this points the vector of states must be sorted!  remove
    // any states with equal z
    m_states.erase(std::unique( m_states.begin(), m_states.end(), equalStateZ ), m_states.end()) ;
    
    // test that there are sufficient states left
    if(m_states.size()<2)
      throw GaudiException("TrackTraj: not enough states for interpolation!","TrackTraj::TrackTraj", StatusCode::FAILURE ) ;
    
    // set the range of the trajectory
    Trajectory::setRange( m_states.front()->z(), m_states.back()->z() ) ;
    
    // initialize the cache
    m_cachedindex = 1 ;
    m_cachedinterpolation.init(*m_states[0],*m_states[1]) ;
    
    // set the field at the first state
    if( magfieldsvc ) 
      magfieldsvc->fieldVector( m_states.front()->position(), m_bfield ).ignore();
  }  

}
