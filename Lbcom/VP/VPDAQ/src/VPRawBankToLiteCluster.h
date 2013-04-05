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
  /// Location in the transient store of the RawEvent object.
  /// @warning Obsolete: use m_rawEventLocations 
  std::string m_rawEventLocation;
  /// List of locations in the transient store to search the RawEvent object.
  std::vector<std::string> m_rawEventLocations;


  bool m_isDebug; 
  bool m_isVerbose;
};

#endif // VPRAWBANKTOLITECLUSTER_H
