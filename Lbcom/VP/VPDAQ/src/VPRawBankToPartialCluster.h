// $Id: VPRawBankToPartialCluster.h,v 1.1 2010-03-01 10:51:28 cocov Exp $
#ifndef VPRAWBANKTOPARTIALCLUSTER_H 
#define VPRAWBANKTOPARTIALCLUSTER_H 1
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
#include "VPPatternWord.h"

/** @class VPRawBankToPartialCluster.h 
 *  VPAlgorithms/VPRawBankToPartialCluster.h
 *
 *  @author Victor Coco
 *  @date   2010/02/24
 */

class VPRawBankToPartialCluster : public GaudiAlgorithm {
public:
  /// Standard constructor
  VPRawBankToPartialCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VPRawBankToPartialCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution
  virtual StatusCode finalize  ();          ///< Algorithm finalise

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VPCluster::Container* clusCont) const;
void createPartialCluster(
               unsigned int sensor,
               VPClusterWord aWord,
               VPPatternWord aPattern,
               LHCb::VPCluster::Container* clusCont) const;

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVP* m_vPelDet;

};

#endif // VPRAWBANKTOLITECLUSTER_H
