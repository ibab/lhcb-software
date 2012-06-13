#ifndef PREPARE_VELOLITE_RAWBANK_H 
#define PREPARE_VELOLITE_RAWBANK_H 1

// STL
#include <vector>
#include <algorithm>
#include <string>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/DigiEvent
#include "Event/VeloStripCluster.h"

class DeVeloLite;

namespace LHCb {
  class RawEvent;
}

/** @class PrepareVeloLiteRawBank PrepareVeloLiteRawBank.h
 *
 */
class PrepareVeloLiteRawBank : public GaudiAlgorithm {

public:
  /// Standard constructor
  PrepareVeloLiteRawBank(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrepareVeloLiteRawBank() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalisation

private:
  
  unsigned int makeBank(LHCb::VeloStripClusters::const_iterator begin, 
                        LHCb::VeloStripClusters::const_iterator end);

  void storeBank(int sensor,
                 LHCb::VeloStripClusters::const_iterator begin,
                 LHCb::VeloStripClusters::const_iterator end,
                 LHCb::RawEvent* rawEvent);

  // Long lived containers for performance reasons 
  std::vector<LHCb::VeloStripCluster*> m_sortedClusters; 
  std::vector<SiDAQ::buffer_word> m_rawData;
  std::vector<SiDAQ::buffer_word> m_clusterADCBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterPosBuffer;

  // Size of raw bank in bytes, including the 4 byte header but
  // *without* the padding bytes at the end
  unsigned int m_bankSizeInBytes;
  // Version number of the raw data format
  unsigned int m_bankVersion;
  /// Pointer to detector element
  DeVeloLite* m_det;
  
  /// List of sensor numbers to check for empty sensors
  std::vector<unsigned int> m_sensorNumbers;

  /// Output level
  bool m_debug;
  bool m_verbose;

};
#endif 
