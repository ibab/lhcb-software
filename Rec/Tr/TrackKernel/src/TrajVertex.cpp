#include "TrackKernel/TrajVertex.h"
#include "TrackKernel/ZTrajectory.h"

namespace LHCb {
  TrajVertex::TrajVertex( const std::vector<const LHCb::ZTrajectory*>& trajectories,
			  double zseed, double ztolerance, double maxdchisq, size_t maxiterations)
    : m_trajectories(trajectories)
  {
    const unsigned int N ( m_trajectories.size() ) ;
    for(unsigned int i = 0; i<N; ++i) 
      addTrack( m_trajectories[i]->state(zseed) ) ;
    
    for(size_t iter = 0; iter<maxiterations; ++iter) {
      if(iter != 0 ) updateStates( zseed ) ;
      TrackStateVertex::fit(maxdchisq,maxiterations) ;
      if( std::abs( zseed - position().z() ) < ztolerance ) break ;
      zseed = position().z() ;
    }
  }
  
  TrajVertex::~TrajVertex() 
  {
  }

  void TrajVertex::updateStates( double zseed )
  {
    const unsigned int N ( m_trajectories.size() ) ;
    for(unsigned int i = 0; i<N; ++i) 
      setInputState( i,  m_trajectories[i]->state(zseed) ) ;
  }

  TrajVertex::FitStatus TrajVertex::fit(double ztolerance, double maxdchisq, size_t maxiterations) 
  { 
    double zseed = position().z() ;
    for(size_t iter = 0; iter<maxiterations; ++iter) {
      updateStates(zseed) ;
      TrackStateVertex::fit(maxdchisq,maxiterations) ;
      if( std::abs( zseed - position().z() ) < ztolerance ) break ;
      zseed = position().z() ;
    }
    return fitStatus() ;
  }
  
  TrajVertex::FitStatus TrajVertex::fitAdaptive( double maxtrkchi2, double ztolerance, 
						 double maxdchisq, size_t maxiterations) 
  {
    double zseed = position().z() ;
    for(size_t iter = 0; iter<2; ++iter) {
      updateStates(zseed) ; // only update states twice
      TrackStateVertex::fitAdaptive(maxtrkchi2,maxdchisq,maxiterations) ;
      if( std::abs( zseed - position().z() ) < ztolerance ) break ;
      zseed = position().z() ;
    } 
    return fitStatus() ;
  }
}
