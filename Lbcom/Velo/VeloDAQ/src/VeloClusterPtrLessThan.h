// $Id: VeloClusterPtrLessThan.h,v 1.2 2006-03-13 18:58:46 krinnert Exp $
#ifndef VELODAQ_VELOCLUSTERPTRLESSTHAN_H 
#define VELODAQ_VELOCLUSTERPTRLESSTHAN_H 1

namespace LHCb {
  class VeloCluster;
}

namespace VeloDAQ {
  /**  Compare two VeloClusters via pointers
   *   This function defines a less-than relation between two
   *   VeloClusters. The lhs cluster is less than the rhs cluster
   *   if the lhs sensor number is lower than the rhs sensor number.
   *   In case of equal sensor numbers, the centroid strip number 
   *   decides.
   *   
   *   @see VeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-23
   */
  bool veloClusterPtrLessThan(const LHCb::VeloCluster* lhs, const LHCb::VeloCluster* rhs); 

}
#endif // VELODAQ_VELOCLUSTERPTRLESSTHAN_H
