#ifndef VELOCLUSTERSTODIGITS_H
#define VELOCLUSTERSTODIGITS_H 1

/** @class VeloClustersToDigits
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

#include "SiClustersToDigits.h"
#include "SiClusterTraits.h"
#include "Event/VeloDigit.h"
typedef SiClustersToDigits<LHCb::VeloCluster> VeloClustersToDigits;
#include "SiClustersToDigits.icpp"

/// specialized constructor
template <>
inline SiClustersToDigits<LHCb::VeloCluster>::SiClustersToDigits( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::VeloCluster>::BASEALG(name, pSvcLocator){
 
  declareProperty("inputLocation", m_inputLocation, LHCb::VeloClusterLocation::Default); 
  declareProperty("outputLocation", m_outputLocation , LHCb::VeloDigitLocation::Default);

 declareProperty("addNeighbourInfo", m_addNeighbourInfo = true);
}

template <>
inline SiClusterTraits<LHCb::VeloCluster>::DIGIT* 
  SiClustersToDigits<LHCb::VeloCluster>::createDigit(const unsigned int adcValue){
  SiClusterTraits<LHCb::VeloCluster>::DIGIT* digit 
   = new SiClusterTraits<LHCb::VeloCluster>::DIGIT();
  digit->setADCValue(adcValue);
  return digit; 
} 

template <>
inline void SiClustersToDigits<LHCb::VeloCluster>::addNeighbours(
   LHCb::VeloCluster* , SiClusterTraits<LHCb::VeloCluster>::DIGIT::Container*){
  // do nothing
}

DECLARE_NAMED_ALGORITHM_FACTORY( VeloClustersToDigits , VeloClustersToDigits )

#endif // VeloClustersToDigits
