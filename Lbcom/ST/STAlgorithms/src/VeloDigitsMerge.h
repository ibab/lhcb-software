#ifndef STDIGITSMERGE_H
#define STDIGITSMERGE_H 1

/** @class STDigitsMerge
 *
 *  simple class for merging cluster containers
 *  ST specialization
 *
 *  @author M.Needham
 *  @date   02/10/2008
 */

// Gaudi
#include "Event/VeloCluster.h"
#include "Event/VeloLiteCluster.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "SiDigitsMerge.h"
#include "SiClusterTraits.h"
typedef SiDigitsMerge<LHCb::VeloCluster> VeloDigitsMerge;
#include "SiDigitsMerge.icpp"

/// specialized constructor
template <>
inline SiDigitsMerge<LHCb::VeloCluster>::SiDigitsMerge( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::VeloCluster>::BASEALG(name, pSvcLocator){
 
  declareProperty("inputLocation", m_inputLocation = LHCb::VeloDigitLocation::Default); 
  declareProperty("outputLocation", m_outputLocation = "/Event/Raw/Velo/MergedDigits");

}

template <>
inline void SiDigitsMerge<LHCb::VeloCluster>::updateCharge(SiClusterTraits<LHCb::VeloCluster>::DIGIT* digit1,
                                                           SiClusterTraits<LHCb::VeloCluster>::DIGIT* digit2){
  const double sumValue = digit1->adcValue() + digit2->adcValue();
  const double newValue = std::max(std::min( m_maxValue,sumValue ) , m_minValue);
  digit1->setADCValue((unsigned short)(newValue));
}

DECLARE_NAMED_ALGORITHM_FACTORY( VeloDigitsMerge , VeloDigitsMerge )

#endif // STDigitsMerge
