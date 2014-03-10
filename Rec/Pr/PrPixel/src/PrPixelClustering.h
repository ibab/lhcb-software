#ifndef PRPIXELCLUSTERING_H 
#define PRPIXELCLUSTERING_H 1
#include <vector>
#include <algorithm>
#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"

#include "Event/VPLiteCluster.h"

#include "PrPixelUtils.h"

// Forward declarations
namespace LHCb {
  class RawEvent ;
}

/** @class PrPixelClustering
 *
 *  Create clusters from raw bank.
 *
 *  This is meant to run after the pattern recognition. It can
 *  create clusters from lite clusters raw banks (version <2)
 *  or super pixel raw banks (versions >=2) transparently.
 *
 *  Note that the PrPixelTracking now runs directly on the raw bank!  So do not
 *  run this algorithm before the pattern recognition in the trigger! It's
 *  supposed to run after an event has passed the first trigger stage.
 *
 *  In case of super pixel raw banks The algorithm utilizes the same core
 *  clustering code as the 3D hit builder in PrPixelHitManager. Differences are
 *  minor and are dealt with in the inc file via the PRPIXELCLUSTERING_CLIENT
 *  preprocessor define.
 *
 *  In case of lite cluster raw banks the decodding is so trivial that we
 *  duplicate thecode.
 *
 *  @author Kurt Rinnert
 *  @date   2014-02-26
 */

class PrPixelClustering : public GaudiAlgorithm {
public:

  /// Comparison functors
  struct LessThanByChannel {
    bool operator() (LHCb::VPLiteCluster lhs, LHCb::VPLiteCluster rhs) const {
      return long(lhs.channelID()) < long(rhs.channelID());
    }
  };

  /// Standard constructor
  PrPixelClustering(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~PrPixelClustering();    ///< Destructor
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute   (); ///< Algorithm execution

private:

/// Wrapper for decoding methods; delegates ddependding on bank version.
StatusCode decodeRawBanks();
/// Decode bank versions < 2. 
StatusCode decodeLCRawBanks(const std::vector<LHCb::RawBank*>& tBanks); 
/// Decode bank versions >= 2. 
StatusCode decodeSPRawBanks(const std::vector<LHCb::RawBank*>& tBanks); 

private:

  std::string m_clusterLocation;
  /// Location in the transient store of the RawEvent object.
  /// @warning Obsolete: use m_rawEventLocations 
  std::string m_rawEventLocation;
  /// List of locations in the transient store to search the RawEvent object.
  std::vector<std::string> m_rawEventLocations;

  bool m_isDebug; 

  LHCb::RawEvent *m_rawEvent;
  LHCb::VPLiteCluster::VPLiteClusters *m_clusters;

  unsigned char m_buffer[PrPixel::SENSOR_PIXELS];
  std::vector<unsigned int> m_pixel_idx;
  std::vector<unsigned int> m_stack;
};

#endif // PRPIXELCLUSTERING_H
