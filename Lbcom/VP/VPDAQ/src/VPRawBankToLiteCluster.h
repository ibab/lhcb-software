// $Id: VPRawBankToLiteCluster.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef VPRAWBANKTOLITECLUSTER_H 
#define VPRAWBANKTOLITECLUSTER_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/RawEvent.h"
#include "Event/VPLiteCluster.h"
// VPelDet
#include "VPDet/DeVP.h"
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"
// Local
#include "VPClusterWord.h"

/** @class VPRawBankToLiteCluster.h 
 *  VPAlgorithms/VPRawBankToLiteCluster.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/12/27
 */

class VPRawBankToLiteCluster : public GaudiAlgorithm {
public:
  /// Standard constructor
  VPRawBankToLiteCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VPRawBankToLiteCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution
  virtual StatusCode finalize  ();          ///< Algorithm finalise

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VPLiteCluster::VPLiteClusters* clusCont) const;
void createLiteCluster(
               unsigned int sensor,
               VPClusterWord aWord,
               LHCb::VPLiteCluster::VPLiteClusters* clusCont) const;

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVP* m_vPelDet;

};

#endif // VPRAWBANKTOLITECLUSTER_H
