#include "StrawAccumulator.h"
#include "StrawHit.h"

namespace OTCal {

  void StrawAccumulator::add(const StrawAccumulator& rhs)
  {
    m_nevents +=                 rhs.m_nevents ;		      
    m_sumtime +=		 rhs.m_sumtime ;		      
    m_sumtimeresidual +=	 rhs.m_sumtimeresidual ;	      
    m_sumrawtimeresidual +=	 rhs.m_sumrawtimeresidual ;
    m_sumrawtimeresidualsq +=	 rhs.m_sumrawtimeresidualsq ;
    m_sumresidual +=		 rhs.m_sumresidual ;	      
    m_sumw +=			 rhs.m_sumw ;		      
    m_sumchisq +=		 rhs.m_sumchisq ;	      
    m_sumresidualsq +=		 rhs.m_sumresidualsq ;         
  }

  inline double sqr(double x) { return x*x ; }

  void StrawAccumulator::add(const StrawHit& hit)
  {
    ++m_nevents ;

    double w  = 1; //1/( hit.trkVariance() ) ;
    m_sumw += w ;
    m_sumtime             += w*hit.driftTime() ;
    m_sumtimeresidual     += w*hit.timeResidual() ;
    m_sumrawtimeresidual  += w*hit.rawTimeResidual() ;
    m_sumrawtimeresidualsq += w*sqr(hit.rawTimeResidual()) ;
    m_sumresidual         += w*hit.residual() ;
    m_sumchisq            += sqr(hit.residual())/hit.residualVariance() ;
    m_sumresidualsq       += sqr(hit.residual()) ;
  }
}
