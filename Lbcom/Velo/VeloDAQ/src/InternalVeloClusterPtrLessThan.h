// $Id:
#ifndef VELODAQ_INTERNALVELOCLUSTERPTRLESSTHAN_H 
#define VELODAQ_INTERNALVELOCLUSTERPTRLESSTHAN_H 1

namespace LHCb {
  class InternalVeloCluster;
}

namespace VeloDAQ {
  /**  Compare two InternalVeloClusters via pointers
   *   This function defines a less-than relation between two
   *   VeloClusters. The lhs cluster is less than the rhs cluster
   *   if the lhs sensor number is lower than the rhs sensor number.
   *   In case of equal sensor numbers, the centroid strip number 
   *   decides.
   *   
   *   @see InternalVeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-0-13
   */
  bool internalVeloClusterPtrLessThan(const LHCb::InternalVeloCluster* lhs, const LHCb::InternalVeloCluster* rhs); 

}
#endif // VELODAQ_INTERNALVELOCLUSTERPTRLESSTHAN_H
