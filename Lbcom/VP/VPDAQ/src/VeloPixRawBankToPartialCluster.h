// $Id: VeloPixRawBankToPartialCluster.h,v 1.1 2010-03-01 10:51:28 cocov Exp $
#ifndef VELOPIXRAWBANKTOPARTIALCLUSTER_H 
#define VELOPIXRAWBANKTOPARTIALCLUSTER_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/RawEvent.h"
#include "Event/VeloPixLiteCluster.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"
// Local
#include "VeloPixClusterWord.h"
#include "VeloPixPatternWord.h"

/** @class VeloPixRawBankToPartialCluster.h 
 *  VeloPixAlgorithms/VeloPixRawBankToPartialCluster.h
 *
 *  @author Victor Coco
 *  @date   2010/02/24
 */

class VeloPixRawBankToPartialCluster : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloPixRawBankToPartialCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VeloPixRawBankToPartialCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution
  virtual StatusCode finalize  ();          ///< Algorithm finalise

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VeloPixCluster::Container* clusCont) const;
void createPartialCluster(
               unsigned int sensor,
               VeloPixClusterWord aWord,
               VeloPixPatternWord aPattern,
               LHCb::VeloPixCluster::Container* clusCont) const;

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVeloPix* m_veloPixelDet;

};

#endif // VELOPIXRAWBANKTOLITECLUSTER_H
