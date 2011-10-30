// $Id$
#ifndef P2PVLOGIC_H 
#define P2PVLOGIC_H 1

// Include files
//DaVinci
#include "Kernel/IDistanceCalculator.h"
/** @file P2PVLogic P2PVLogic.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-20
 */

/**@struct _p2PVWithIP 
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-20
 */

struct _p2PVWithIP 
{
  static double weight(const LHCb::Particle* particle,
                       const LHCb::VertexBase* pv,
                       const IDistanceCalculator* distCalc) 
  {
    double fom(0.);
    const StatusCode sc =  distCalc->distance(particle, pv, fom);
    return ( (sc.isSuccess()) 
             && fom > std::numeric_limits<double>::epsilon() ) 
      ? 1./fom : 0. ;
  }
  
};

/**@struct _p2PVWithIPChi2
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-20
 */

struct _p2PVWithIPChi2
{
  static double weight(const LHCb::Particle* particle,
                       const LHCb::VertexBase* pv,
                       const IDistanceCalculator* distCalc) 
  {
    double fom(0.);
    double chi2(0.);
    const StatusCode sc = distCalc->distance(particle, pv, fom, chi2);
    return ( (sc.isSuccess()) 
             && chi2 > std::numeric_limits<double>::epsilon() ) 
      ?  1./chi2 : 0 ;
  }
};

#endif // P2PVLOGIC_H
