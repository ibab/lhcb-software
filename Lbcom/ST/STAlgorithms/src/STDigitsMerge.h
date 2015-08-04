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
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Kernel/STAlgBase.h"

#include "SiDigitsMerge.h"
#include "SiClusterTraits.h"
typedef SiDigitsMerge<LHCb::STCluster> STDigitsMerge;
#include "SiDigitsMerge.icpp"

template <>
inline SiDigitsMerge<LHCb::STCluster>::SiDigitsMerge( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  SiClusterTraits<LHCb::STCluster>::BASEALG(name, pSvcLocator),
  m_inputLocation(LHCb::STDigitLocation::TTDigits){
 
  declareProperty( "nBits" , m_nBits = 6u);
  declareProperty("spills", m_spillsVector);
 
  declareSTConfigProperty("outputLocation", m_outputLocation , "/Event/Raw/TT/MergedDigits");
  addToFlipList(&m_inputLocation);
}

template <>
inline void SiDigitsMerge<LHCb::STCluster>::updateCharge(SiClusterTraits<LHCb::STCluster>::DIGIT* digit1,
                                                         SiClusterTraits<LHCb::STCluster>::DIGIT* digit2){
  const double sumvalue = digit1->depositedCharge() + digit2->depositedCharge();
  const double newvalue = std::max(std::min( m_maxValue,sumvalue ) , m_minValue);
  digit1->setDepositedCharge(newvalue);
}

DECLARE_NAMED_ALGORITHM_FACTORY( STDigitsMerge , STDigitsMerge )

#endif // STDigitsMerge
