#ifndef VELOCLUSTERSTOLITE_H
#define VELOCLUSTERSTOLITE_H 1

/** @class VeloClustersToLite
 *
 *  simple class for stripping liteCluster from clusters
 *  ST specialization
 *
 *  @author M.Needham
 *  @date   02/10/2008
 */

// Gaudi
#include "Event/VeloCluster.h"
#include "Event/VeloLiteCluster.h"
#include "Kernel/STAlgBase.h"

#include "SiClustersToLite.h"
#include "SiClusterTraits.h"
typedef SiClustersToLite<LHCb::VeloCluster> VeloClustersToLite;
#include "SiClustersToLite.icpp"

/// specialized constructor
template <>
inline SiClustersToLite<LHCb::VeloCluster>::SiClustersToLite( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::VeloCluster>::BASEALG(name, pSvcLocator){
 
  declareProperty("inputLocation", m_inputLocation, LHCb::VeloClusterLocation::Default); 
  declareProperty("outputLocation", m_outputLocation , LHCb::VeloLiteClusterLocation::Default);

}

DECLARE_NAMED_ALGORITHM_FACTORY( VeloClustersToLite , VeloClustersToLite )

#endif // VeloClustersToLite
