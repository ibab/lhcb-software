// $Id: sumMomenta.h,v 1.1 2006-05-09 16:21:27 pkoppenb Exp $
#ifndef KERNEL_SUMMOMENTA_H 
#define KERNEL_SUMMOMENTA_H 1

// Include files
#include "Event/Particle.h"

/** @class sumMomenta sumMomenta.h Kernel/sumMomenta.h
 *  
 *  Trivial functions for summing momenta 
 * 
 *  @author Patrick Koppenburg
 *  @date   2006-05-05
 */
//=============================================================================
/// Sum 4-Momenta
//=============================================================================
Gaudi::XYZTVector sumMomenta( const LHCb::Particle::ConstVector& vP ){
  Gaudi::XYZTVector sum;
  for ( LHCb::Particle::ConstVector::const_iterator i = vP.begin(); i != vP.end() ; ++i){
    sum += (*i)->momentum();
  }
  return sum;
}
//=============================================================================
/// Sum 4-Momenta
//=============================================================================
Gaudi::XYZTVector sumMomenta( const SmartRefVector<LHCb::Particle>& vP ){
  Gaudi::XYZTVector sum;
  for ( SmartRefVector<LHCb::Particle>::const_iterator i = vP.begin(); i != vP.end() ; ++i){
    sum += (*i)->momentum();
  }
  return sum;
}

#endif // KERNEL_SUMMOMENTA_H
