#ifndef TRT_ALIGNTOOLS__STRAWACCUMULATOR_H
#define TRT_ALIGNTOOLS__STRAWACCUMULATOR_H

#include <math.h>
#include "StrawHit.h"

namespace OTCal 
{
  class StrawAccumulator
  {
  public:
    StrawAccumulator() { reset() ; }
    double avtimeresidual() const { return m_sumtimeresidual/m_sumw ; }
    double avrawtimeresidual() const { return m_sumrawtimeresidual/m_sumw ; }
    double sumw() const { return m_sumw ; }
    double chisq() const { return m_sumchisq/m_nevents ; }
    double residualrms() const { return std::sqrt( ( m_sumresidualsq - m_sumresidual*m_sumresidual/m_nevents)/m_nevents) ; }
    unsigned int nentries() const { return m_nevents ; }
    double avrawtimeresidualerror() const ;
    
    void add(const StrawHit& hit) ;
    void add(const StrawAccumulator& rhs) ;

    void reset() {
      m_nevents = 0 ;
      m_sumtime = m_sumrawtime = m_sumtimeresidual = m_sumrawtimeresidual = m_sumrawtimeresidualsq =
	m_sumresidual = m_sumw = m_sumchisq = m_sumresidualsq = 0 ;
    }
    
  private:
    unsigned int m_nevents ;
    double m_sumtime ;
    double m_sumrawtime ;
    double m_sumtimeresidual ;
    double m_sumrawtimeresidual ;
    double m_sumrawtimeresidualsq ;
    double m_sumresidual ;
    double m_sumw ;
    double m_sumchisq ;
    double m_sumresidualsq ;
  } ;
  
  inline double StrawAccumulator::avrawtimeresidualerror() const { 
    // if we'd really did a chisquare fit, the error would be simply
    // the average inverse weight, provided it were calculated in the
    // proper units. however, the weight have a rather arbitrary scale
    // here. therefore, we'll just return rms/sqrt(N), sort of.
    double rms = std::sqrt( ( m_sumrawtimeresidualsq - m_sumrawtimeresidual*m_sumrawtimeresidual/m_sumw)/m_sumw ) ;
    return rms / std::sqrt( (double) m_nevents) ;
  }


}

#endif
