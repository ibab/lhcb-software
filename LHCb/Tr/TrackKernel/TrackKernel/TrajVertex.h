#ifndef __TRACKKERNEL_TRACKTRAJVERTEX_H__
#define __TRACKKERNEL_TRACKTRAJVERTEX_H__

#include "TrackKernel/TrackStateVertex.h"

namespace LHCb {

  class ZTrajectory ;

  class TrajVertex : public TrackStateVertex
  {
  public:
    TrajVertex( const std::vector<const LHCb::ZTrajectory*>& trajectories,
		double zseed, double maxdchisq=0.01, size_t maxiterations=10 ) ;
    ~TrajVertex() ;
    
    typedef std::vector<const LHCb::ZTrajectory*> Trajectories ;
    const Trajectories& trajectories() const { return m_trajectories ; }
    
  private:
    Trajectories m_trajectories ;
  } ;
  
}
#endif
