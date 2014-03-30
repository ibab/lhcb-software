#ifndef VPRAWBANKTOPARTIALCLUSTER_H 
#define VPRAWBANKTOPARTIALCLUSTER_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/VPCluster.h"
// Local
#include "VPClusterWord.h"
#include "VPPatternWord.h"
#include "DAQKernel/DecoderAlgBase.h"

// Forward declarations
class DeVP;
namespace LHCb {
  class RawEvent ;
}
/** @class VPRawBankToPartialCluster.h 
 *  VPAlgorithms/VPRawBankToPartialCluster.h
 *
 *  @author Victor Coco
 *  @date   2010/02/24
 */

class VPRawBankToPartialCluster : public Decoder::AlgBase {
public:
  /// Standard constructor
  VPRawBankToPartialCluster(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VPRawBankToPartialCluster();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt, LHCb::VPClusters* clusters) const;
void createPartialCluster(unsigned int sensor,
                          VPClusterWord aWord,
                          LHCb::VPClusters* clusters) const;

private:

  std::string m_clusterLocation;
  
  bool m_isDebug; 
  DeVP* m_det;

};

#endif // VPRAWBANKTOPARTIALCLUSTER_H
