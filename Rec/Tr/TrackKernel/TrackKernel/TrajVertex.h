#ifndef __TRACKKERNEL_TRACKTRAJVERTEX_H__
#define __TRACKKERNEL_TRACKTRAJVERTEX_H__

#include "TrackKernel/TrackStateVertex.h"

namespace LHCb {

  class ZTrajectory ;

  class TrajVertex : public TrackStateVertex
  {
  public:
    TrajVertex( const std::vector<const LHCb::ZTrajectory*>& trajectories,
		double zseed, 
		double ztolerance = 10, // mm
		double maxdchisq=0.01, size_t maxiterations=10 ) ;
    ~TrajVertex() ;
    
    typedef std::vector<const LHCb::ZTrajectory*> Trajectories ;
    const Trajectories& trajectories() const { return m_trajectories ; }
    
       /// fit until converged
    FitStatus fit( double ztolerance = 10, double maxdchisq=0.01, size_t maxiterations=10) ;

    /// adapative fit. downweight tracks with chi2 contribution larger than maxtrkchi2
    FitStatus fitAdaptive( double maxtrkchi2=4, double ztolerance = 10, double maxdchisq=0.01, size_t maxiterations=10) ;

  private:
    void updateStates( double z ) ;
  private:
    Trajectories m_trajectories ;
  } ;
  
}
#endif
