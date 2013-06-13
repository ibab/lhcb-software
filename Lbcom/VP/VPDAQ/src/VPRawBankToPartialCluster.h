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

class VPRawBankToPartialCluster : public GaudiAlgorithm {
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
  /// Location in the transient store of the RawEvent object.
  /// @warning Obsolete: use m_rawEventLocations 
  std::string m_rawEventLocation;
  /// List of locations in the transient store to search the RawEvent object.
  std::vector<std::string> m_rawEventLocations;

  bool m_isDebug; 
  DeVP* m_vPelDet;

};

#endif // VPRAWBANKTOLITECLUSTER_H
