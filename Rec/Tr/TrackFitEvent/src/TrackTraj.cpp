
#include "Event/TrackTraj.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

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
    // the sorting takes a lot of time. one solution is to assume
    // things are sorted and then use 'std::merge' to merge the sorted
    // vectors. I'll htink about that another time.

    // first add the states from the nodes. make sure these are sorted
    // in increasing z
    m_states.reserve( track.nodes().size() + track.states().size() ) ;
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
    
    // then add remaining states (doing it in this order, makes the
    // ordering best). the best thing is to use 'std::merge' now.
    for(std::vector<State*>::const_iterator it = track.states().begin() ;
        it != track.states().end(); ++it) 
      m_states.push_back( *it ) ;
    
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  TrackTraj::TrackTraj(const TrackTraj::StateContainer& states, const IMagneticFieldSvc* magfieldsvc)
    : Trajectory(0,0),m_states(states), m_bfield(0,0,0)
  {
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  void TrackTraj::init(const IMagneticFieldSvc* magfieldsvc)
  {
    // sort the states
    std::sort(m_states.begin(), m_states.end(),compareStateZ) ;
    
    // remove any states with equal z (requires sorted vector!)
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
