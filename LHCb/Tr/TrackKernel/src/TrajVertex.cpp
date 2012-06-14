#include "TrackKernel/TrajVertex.h"
#include "TrackKernel/ZTrajectory.h"

namespace LHCb {
  TrajVertex::TrajVertex( const std::vector<const LHCb::ZTrajectory*>& trajectories,
				    double zseed, double maxdchisq, size_t maxiterations) 
    : m_trajectories(trajectories)
  {
    const unsigned int N ( m_trajectories.size() ) ;
    for(unsigned int i = 0; i<N; ++i) 
      addTrack( m_trajectories[i]->state(zseed) ) ;
    
    for(size_t iter = 0; iter<maxiterations; ++iter) {
      if(iter != 0 ) {
	for(unsigned int i = 0; i<N; ++i) 
	  setInputState( i,  m_trajectories[i]->state(zseed) ) ;
      }
      fit(maxdchisq,maxiterations) ;
      if( std::abs( zseed - position().z() ) < 10 ) break ;
      zseed = position().z() ;
    }
  }
  
  TrajVertex::~TrajVertex() 
  {
  }
  
}
