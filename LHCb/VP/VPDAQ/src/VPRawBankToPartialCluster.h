#ifndef VPRAWBANKTOPARTIALCLUSTER_H 
#define VPRAWBANKTOPARTIALCLUSTER_H 1
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

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt,
           LHCb::VPCluster::Container* clusCont) const;
void createPartialCluster(
               unsigned int sensor,
               VPClusterWord aWord,
               VPPatternWord aPattern,
               LHCb::VPCluster::Container* clusCont) const;

private:

  std::string m_clusterLocation;
  
  bool m_isDebug; 
  DeVP* m_vPelDet;

};

#endif // VPRAWBANKTOLITECLUSTER_H
