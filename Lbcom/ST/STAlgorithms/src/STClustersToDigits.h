#ifndef STCLUSTERSTODIGITS_H
#define STCLUSTERSTODIGITS_H 1

/** @class STClustersToDigits
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

#include "SiClustersToDigits.h"
#include "SiClusterTraits.h"
typedef SiClustersToDigits<LHCb::STCluster> STClustersToDigits;
#include "SiClustersToDigits.icpp"


template <>
inline SiClustersToDigits<LHCb::STCluster>::SiClustersToDigits( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::STCluster>::BASEALG(name, pSvcLocator){
 
  declareSTConfigProperty("inputLocation", m_inputLocation, LHCb::STClusterLocation::TTClusters); 
  declareSTConfigProperty("outputLocation", m_outputLocation , LHCb::STDigitLocation::TTDigits);

  declareProperty("addNeighbourInfo", m_addNeighbourInfo = true);
  setForcedInit();
}

template <>
inline SiClusterTraits<LHCb::STCluster>::DIGIT* 
  SiClustersToDigits<LHCb::STCluster>::createDigit(const unsigned int adcValue){
  return new SiClusterTraits<LHCb::STCluster>::DIGIT(adcValue);
} 

#include "STDet/DeSTDetector.h"

template <>
inline void SiClustersToDigits<LHCb::STCluster>::addNeighbours(
  LHCb::STCluster* clus ,SiClusterTraits<LHCb::STCluster>::DIGIT::Container* digitCont){
  
  // use the neighbour sum to add information on the neighbouring strips..

  // left + right neighbour
  LHCb::STChannelID leftChan = tracker()->nextLeft(clus->firstChannel());
  LHCb::STChannelID rightChan = tracker()->nextRight(clus->firstChannel());
 
  double neighbourSum = clus->neighbourSum(); 
  if (leftChan != 0u && rightChan != 0u){
    neighbourSum *= 0.5;
    if (digitCont->object(leftChan) == 0) {
      digitCont->insert(new LHCb::STDigit(neighbourSum),leftChan);
    } 
    if (digitCont->object(rightChan) == 0) {
      digitCont->insert(new LHCb::STDigit(neighbourSum),rightChan);
    } 
  }
  else if (leftChan !=0u){
     if (digitCont->object(leftChan) == 0) {
      digitCont->insert(new LHCb::STDigit(neighbourSum),leftChan);
    } 
  }
  else if (rightChan !=0u){
    if (digitCont->object(rightChan) == 0) {
      digitCont->insert(new LHCb::STDigit(neighbourSum),rightChan);
    } 
  } 
  else {
    // nothing
  }
}

DECLARE_NAMED_ALGORITHM_FACTORY( STClustersToDigits , STClustersToDigits )

#endif // STClustersToDigits
