#ifndef PRPIXELCLUSTERING_H 
#define PRPIXELCLUSTERING_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/VPLiteCluster.h"

// Forward declarations
class DeVP;
namespace LHCb {
  class RawEvent ;
}

/** @class PrPixelClustering.h 
 *  PrPixel/PrPixelClustering.h
 *
 *  Create clusters from super pixel raw bank.
 *
 *  This is meant to run after the pattern recognition when
 *  using the super pixel raw bank.
 *
 *  Note that the PrPixelTracking runs directly on the raw super
 *  pixel bank! So do not run this algorithm before the pattern
 *  recognition in the trigger! It's supposed to run after an event
 *  has passed the first trigger stage.
 *
 *  The algorithm utilizes the same core clustering code as the 3D
 *  hit builder in PrPixelHitManager. Differences are minor and are
 *  dealt with in the inc file via the PRPIXELCLUSTERING_CLIENT 
 *  preprocessor define.
 *
 *  @author Kurt Rinnert
 *  @date   2014-02-26
 */

class PrPixelClustering : public GaudiAlgorithm {
public:

  /// Useful constants
  enum { 
    MODULE_SENSORS = 4,
    SENSOR_CHIPS = 3,
    CHIP_ROWS = 256,
    CHIP_COLUMNS = 256,
    CHIP_0_END = 255,
    CHIP_1_START = 256,
    CHIP_1_END = 511,
    CHIP_2_START = 512,
    SENSOR_ROWS = 256, 
    SENSOR_COLUMNS = 768, 
    SENSOR_PIXELS = SENSOR_ROWS*SENSOR_COLUMNS
  };

  /// Comparison functors
  struct LessThanByChannel {
    bool operator() (LHCb::VPLiteCluster lhs, LHCb::VPLiteCluster rhs) const {
      return long(lhs.channelID()) < long(rhs.channelID());
    }
  };

  /// Standard constructor
  PrPixelClustering(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~PrPixelClustering();   ///< Destructor
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute   (); ///< Algorithm execution

private:

StatusCode decodeRawBanks();

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

  unsigned char m_buffer[SENSOR_PIXELS];
  std::vector<unsigned int> m_pixel_idx;
  std::vector<unsigned int> m_stack;
};

#endif // PRPIXELCLUSTERING_H
