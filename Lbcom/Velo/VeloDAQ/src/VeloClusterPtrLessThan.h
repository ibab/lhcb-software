// $Id: VeloClusterPtrLessThan.h,v 1.1 2006-02-23 18:56:35 krinnert Exp $
#ifndef VELODAQ_VELOCLUSTERPTRLESSTHAN_H 
#define VELODAQ_VELOCLUSTERPTRLESSTHAN_H 1

namespace LHCb {
  class VeloCluster;
}

namespace VeloDAQ {
  /**  Compare two VeloClusters via pointers
   *   This function defines a less-than relation between two
   *   VeloClusters. The current implementation simply compares
   *   the strip numbers from the channel IDs.
   *   
   *   @see VeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-23
   */
  bool veloClusterPtrLessThan(const LHCb::VeloCluster* lhs, const LHCb::VeloCluster* rhs); 

}
#endif // VELODAQ_VELOCLUSTERPTRLESSTHAN_H
