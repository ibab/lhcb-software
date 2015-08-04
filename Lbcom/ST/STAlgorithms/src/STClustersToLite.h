#ifndef STCLUSTERSTOLITE_H
#define STCLUSTERSTOLITE_H 1

/** @class STClustersToLite
 *
 *  simple class for stripping liteCluster from clusters
 *  ST specialization
 *
 *  @author M.Needham
 *  @date   02/10/2008
 */

// Gaudi
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Kernel/STAlgBase.h"

#include "SiClustersToLite.h"
#include "SiClusterTraits.h"
typedef SiClustersToLite<LHCb::STCluster> STClustersToLite;
#include "SiClustersToLite.icpp"


template <>
inline SiClustersToLite<LHCb::STCluster>::SiClustersToLite( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::STCluster>::BASEALG(name, pSvcLocator){
 
  // These cannot be STConfig properties due to DoD service requirements.
  declareProperty("inputLocation", m_inputLocation=  LHCb::STClusterLocation::TTClusters); 
  declareProperty("outputLocation", m_outputLocation = LHCb::STLiteClusterLocation::TTClusters);

}

DECLARE_NAMED_ALGORITHM_FACTORY( STClustersToLite , STClustersToLite )

#endif // STClustersToLite
