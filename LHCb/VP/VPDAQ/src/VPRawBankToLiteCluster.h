#ifndef VPRAWBANKTOLITECLUSTER_H 
#define VPRAWBANKTOLITECLUSTER_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/VPLiteCluster.h"
// Local
#include "VPClusterWord.h"
#include "DAQKernel/DecoderAlgBase.h"

// Forward declarations
class DeVP;
namespace LHCb {
  class RawEvent ;
}

/** @class VPRawBankToLiteCluster.h 
 *  VPAlgorithms/VPRawBankToLiteCluster.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/12/27
 */

class VPRawBankToLiteCluster : public Decoder::AlgBase {
public:
  /// Standard constructor
  VPRawBankToLiteCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VPRawBankToLiteCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VPLiteCluster::VPLiteClusters* clusCont) const;
void createLiteCluster(
               unsigned int sensor,
               VPClusterWord aWord,
               LHCb::VPLiteCluster::VPLiteClusters* clusCont) const;

private:

  std::string m_clusterLocation;
  
  bool m_isDebug; 
};

#endif // VPRAWBANKTOLITECLUSTER_H
