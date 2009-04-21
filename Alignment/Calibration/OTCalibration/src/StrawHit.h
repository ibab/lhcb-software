#ifndef OTCALIBRATION_STRAWHIT_H
#define OTCALIBRATION_STRAWHIT_H

#include <cmath>
#include <Kernel/OTChannelID.h>

class DeOTDetector ;
class DeOTModule ;
class ITrackProjector ;
namespace LHCb
{
  class Node ; 
}

namespace OTCal
{
  class StrawHit
  {
  public:
    StrawHit(const ITrackProjector& projector, const LHCb::Node& node )  ;
    //StrawHit( const DeOTDetector& otdet, const LHCb::Node& node) ;
    const DeOTModule* module() const { return m_module ; }
    double trkDistance() const { return m_trkdistance ; }
    double trkVariance() const { return m_trkcov ; }
    double driftTime() const { return m_driftTime ; }
    double residual() const { return m_residual ; }
    double residualVariance() const { return m_residualcov ; }
    double timeResidual() const { return m_driftTime - m_trkDriftTime ; }
    double driftRadius() const { return m_driftRadius ; }
    double residualPull() const { return residual() / std::sqrt(residualVariance())  ; }
    double chisq() const { return residual()*residual() / residualVariance() ; }
    double residualScaleFactor() const { return std::sqrt( m_hitcov / m_residualcov ) ; }
    double rawTimeResidual() const { return m_t0estimate ; }
    unsigned int straw() const { return m_channelid.straw() ; }

    unsigned int uniqueLayer() const ;
    unsigned int uniqueQuarter() const ;
    unsigned int uniqueModule() const ;
    unsigned int uniqueOtis() const ;
    
    bool isOutlier() const { return m_isOutlier ; }
  private:
    const DeOTModule* m_module ;
    LHCb::OTChannelID m_channelid ;
    bool m_isOutlier ;
    // this comes from the node. this is already 'unbiased'
    double m_trkdistance ;
    double m_trkcov ;
    double m_residual ;
    double m_residualcov ;
    double m_hitcov ;
    double m_trkDriftTime ;
    // this comes from the hit
    double m_driftTime ;
    double m_driftRadius ;
    double m_t0estimate ;
  } ;
} 

  
#endif
