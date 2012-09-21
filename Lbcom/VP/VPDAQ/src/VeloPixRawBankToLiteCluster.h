// $Id: VeloPixRawBankToLiteCluster.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef VELOPIXRAWBANKTOLITECLUSTER_H 
#define VELOPIXRAWBANKTOLITECLUSTER_H 1
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

/** @class VeloPixRawBankToLiteCluster.h 
 *  VeloPixAlgorithms/VeloPixRawBankToLiteCluster.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/12/27
 */

class VeloPixRawBankToLiteCluster : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloPixRawBankToLiteCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VeloPixRawBankToLiteCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution
  virtual StatusCode finalize  ();          ///< Algorithm finalise

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VeloPixLiteCluster::VeloPixLiteClusters* clusCont) const;
void createLiteCluster(
               unsigned int sensor,
               VeloPixClusterWord aWord,
               LHCb::VeloPixLiteCluster::VeloPixLiteClusters* clusCont) const;

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVeloPix* m_veloPixelDet;

};

#endif // VELOPIXRAWBANKTOLITECLUSTER_H
