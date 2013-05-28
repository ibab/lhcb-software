
#include "Event/Node.h"
#include "Event/TrackFitResult.h"
#include "TrackKernel/TrackTraj.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include <algorithm>
#include "gsl/gsl_integration.h"

#ifdef __INTEL_COMPILER       // Disable ICC warning
  #pragma warning(disable:68) // integer conversion resulted in a change of sign
#endif

namespace LHCb
{
  
  inline bool compareStateZ(const State* lhs, const State* rhs)
  {
    return lhs->z() < rhs->z() ;
  }
  
  inline bool equalStateZ(const State* lhs, const State* rhs)
  {
    return std::abs(lhs->z() - rhs->z()) < TrackParameters::propagationTolerance ;
  }
  
  TrackTraj::TrackTraj(const Track& track, const IMagneticFieldSvc* magfieldsvc)
    : ZTrajectory(), m_bfield(0,0,0),
      m_cachedindex(InvalidCacheIndex)
  {
    // the sorting takes a lot of time. therefore, we rely on the fact
    // that nodes and states in a track are already sorted. we use
    // 'std::merge' to merge the sorted ranges.
    
    // first add the states from the track.nodes(). make sure these
    // are ordered in increasing z.
    StateContainer statesfromnodes ;
    if( track.fitResult() ) {
      const LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes() ;
      if( ! nodes.empty() ) {
	statesfromnodes.reserve( nodes.size() ) ;
	if( nodes.front()->z() < nodes.back()->z() ) {
	  //nodes in right order
	  for(LHCb::TrackFitResult::NodeContainer::const_iterator it = nodes.begin() ;
	      it != nodes.end(); ++it) 
	    statesfromnodes.push_back( &((*it)->state() )) ;
	} else {
	  // nodes in wrong order
	  for(LHCb::TrackFitResult::NodeContainer::const_reverse_iterator it = nodes.rbegin() ;
	      it != nodes.rend(); ++it) 
	    statesfromnodes.push_back( &((*it)->state() )) ;
	}
      }
    }
    StateContainer statesfromstates(track.states().begin(),track.states().end()) ;
    // states on backward tracks are in reverse order
    if( !statesfromstates.empty() && statesfromstates.front()->z() > statesfromstates.back()->z() )
      std::reverse( statesfromstates.begin(), statesfromstates.end() ) ;
    
    // now use std::merge to add track.states(). we could also use
    // inplace_merge: then we don't need to create a vector for the
    // nodes first. I don't know what is faster.
    m_states.resize( statesfromnodes.size() + statesfromstates.size() ) ;
    std::merge( statesfromnodes.begin(), statesfromnodes.end(),
		statesfromstates.begin(), statesfromstates.end(),
		m_states.begin(), compareStateZ ) ;
    
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  TrackTraj::TrackTraj(const std::vector<Node*>& nodes, const IMagneticFieldSvc* magfieldsvc)
    : ZTrajectory(), m_bfield(0,0,0),
      m_cachedindex(InvalidCacheIndex)
  {
    // the sorting takes a lot of time. therefore, we rely on the fact
    // that nodes and states in a track are already sorted. 
    
    // first add the states from the track.nodes(). make sure these
    // are ordered in increasing z.
    if(!nodes.empty()) {
      m_states.reserve( nodes.size() ) ;
      if( nodes.front()->z() < nodes.back()->z() ) {
	//nodes in right order
	for(std::vector<Node*>::const_iterator it = nodes.begin() ;
	    it != nodes.end(); ++it) 
	  m_states.push_back( &((*it)->state() )) ;
      } else {
	// nodes in wrong order
	for(std::vector<Node*>::const_reverse_iterator it = nodes.rbegin() ;
	    it != nodes.rend(); ++it) 
	  m_states.push_back( &((*it)->state() )) ;
      }
    }
    
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  TrackTraj::TrackTraj(const TrackTraj::StateContainer& states, const IMagneticFieldSvc* magfieldsvc)
    : ZTrajectory(),m_states(states), m_bfield(0,0,0),
      m_cachedindex(InvalidCacheIndex)
  {
    // sort
    std::sort(m_states.begin(), m_states.end(),compareStateZ) ;
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  TrackTraj::TrackTraj(const LHCb::Track::StateContainer& states, const IMagneticFieldSvc* magfieldsvc)
    : ZTrajectory(), m_bfield(0,0,0),
      m_cachedindex(InvalidCacheIndex)
  {
    // insert
    m_states.insert(m_states.begin(),states.begin(),states.end()) ;
    // reverse, e.g. for backward tracks
    if( !m_states.empty() && m_states.front()->z() > m_states.back()->z() )
      std::reverse( m_states.begin(), m_states.end() ) ;
    // check states and initialize cache
    init(magfieldsvc) ;
  }
  
  void TrackTraj::init(const IMagneticFieldSvc* magfieldsvc)
  {
    // add this points the vector of states must be sorted!  remove
    // any states with equal z
    m_states.erase(std::unique( m_states.begin(), m_states.end(), equalStateZ ), m_states.end()) ;
    
    // test that there are sufficient states left
    if(m_states.empty())
      throw GaudiException("TrackTraj: not enough states for interpolation!","TrackTraj::TrackTraj", StatusCode::FAILURE ) ;
    
    // set the range of the trajectory
    Trajectory::setRange( m_states.front()->z(), m_states.back()->z() ) ;
    
    // set the field at the first state
    if( magfieldsvc ) 
      magfieldsvc->fieldVector( m_states.front()->position(), m_bfield ).ignore();

    // invalidate the cache
    invalidateCache() ;
  }


  void TrackTraj::updatecache(double z) const
  {
    // m_cachedindex==0: before first state
    // m_cachedindex==[1,...,numstates-1] --> between states
    // m_cachedindex==numstates --> after last state
    // m_cachedindex==INVALIDCACHEINDEX --> cache is not valid
    bool cacheisvalid = 
      (m_cachedindex != size_t(InvalidCacheIndex)) &&
      (
       (m_cachedindex==0 && z <= m_states.front()->z()) ||
       (m_cachedindex==m_states.size() && z >= m_states.back()->z()) ||
       (m_cachedindex!=0 && m_states[m_cachedindex-1]->z() <= z && z < m_states[m_cachedindex]->z())
       ) ;
    
    if( !cacheisvalid ) {
      if( z <= m_states.front()->z() ) {
	m_cachedindex=0 ;
        m_cachedinterpolation.init(*m_states.front(), m_bfield ) ;
      } else if( z >= m_states.back()->z() ) {
	m_cachedindex=m_states.size() ;
        m_cachedinterpolation.init(*m_states.back(), Gaudi::XYZVector(0,0,0) ) ;
      } else {
        m_cachedindex = 1 ;
        while( m_cachedindex < m_states.size()-1 &&
               m_states[m_cachedindex]->z() <= z) ++m_cachedindex ;
        m_cachedinterpolation.init(*m_states[m_cachedindex-1],*m_states[m_cachedindex]) ;
      }
    }
  }


  // Copied from Gerhard
  class ArcLengthComputer
  {
  private:
    typedef LHCb::TrackTraj param_t;
    gsl_integration_workspace * m_workspace ;
    size_t m_limit ;
    static double GSLgluefun(double z, void *x) { return static_cast<param_t*>(x)->dArclengthDMu(z) ; }
  public:
    ArcLengthComputer() : m_limit(1000) {
      m_workspace = gsl_integration_workspace_alloc (m_limit) ;
    }
    ~ArcLengthComputer() {
      gsl_integration_workspace_free (m_workspace) ;
    }
    double compute(const TrackTraj& traj, double z1, double z2) const
    {
      const double epsAbs = 1*Gaudi::Units::cm ;
      const double epsRel = 1e-3 ;
      gsl_function f ;
      f.function = &ArcLengthComputer::GSLgluefun;
      f.params   = const_cast<param_t*>( &traj );
      double result, error;
      //size_t neval; 
      //gsl_integration_qng(&f, z1, z2, epsAbs, epsRel,&result, &error,&neval);
      //std::cout << "ArcLengthComputer::compute: " << error << " " << neval << std::endl ;
      const int key=2 ;
      gsl_integration_qag(&f,z1,z2,epsAbs,epsRel,m_limit, key, m_workspace, &result, &error) ;
      //std::cout << "ArcLengthComputer::compute: " << error << std::endl ;
      return result ;
    }
  } ;
  
  double TrackTraj::arclength( double z1, double z2 ) const 
  { 
    static ArcLengthComputer computer ;
    return computer.compute(*this,z1,z2) ;
  } 

  std::vector<StateVector> TrackTraj::refStateVectors() const 
  { 
    std::vector<StateVector> states ;
    states.reserve(m_states.size()) ;
    for( StateContainer::const_iterator it = m_states.begin() ;
	 it != m_states.end(); ++it) 
      states.push_back( StateVector((*it)->stateVector(),(*it)->z()) ) ;
    return states ;
  }

  double TrackTraj::distTo1stError( double z, double tolerance, int pathDirection) const 
  {
    // WH: if timing is ever an issue, we should probably just return 'a' and not care about boundaries.
    updatecache(z) ;
    const double a  = m_cachedinterpolation.distTo1stError(z,tolerance,pathDirection) ;
    const bool   extrapolate = z <=  m_states.front()->z() || z >= m_states.back()->z() ;
    // add fudge factor to make sure we step across boundaries
    const double fudgefactor = 1.01 ;
    return fudgefactor * ( extrapolate ? a : 
			   std::min( a, pathDirection > 0 ? 
				     m_states[m_cachedindex]->z() - z : 
				     z - m_states[m_cachedindex-1]->z() ) );
  }

  double TrackTraj::distTo2ndError( double z, double tolerance, int pathDirection) const 
  {
    // WH: if timing is ever an issue, we should probably just return 'a' and not care about boundaries.
    updatecache(z) ;
    const double a  = m_cachedinterpolation.distTo2ndError(z,tolerance,pathDirection) ;
    const bool   extrapolate = z <=  m_states.front()->z() || z >= m_states.back()->z() ;
    // add fudge factor to make sure we step across boundaries
    const double fudgefactor = 1.01 ;
    return fudgefactor *  ( extrapolate ? a : 
			    std::min( a, pathDirection > 0 ? 
				      m_states[m_cachedindex]->z() - z : 
				      z - m_states[m_cachedindex-1]->z() ) );
  }
  
}
