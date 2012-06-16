#ifndef ENCODE_VL_RAWBANK_H 
#define ENCODE_VL_RAWBANK_H 1

// STL
#include <vector>
#include <algorithm>
#include <string>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/DigiEvent
#include "Event/VLCluster.h"

class DeVL;

namespace LHCb {
  class RawEvent;
}

/** @class VLRawBankEncoder VLRawBankEncoder.h
 *
 * Write out VLClusters to raw banks
 */

class VLRawBankEncoder : public GaudiAlgorithm {

public:
  /// Standard constructor
  VLRawBankEncoder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLRawBankEncoder() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalisation

private:
  
  unsigned int makeBank(LHCb::VLClusters::const_iterator begin, 
                        LHCb::VLClusters::const_iterator end);

  void storeBank(int sensor,
                 LHCb::VLClusters::const_iterator begin,
                 LHCb::VLClusters::const_iterator end,
                 LHCb::RawEvent* rawEvent);

  std::vector<LHCb::VLCluster*> m_sortedClusters; 
  std::vector<VLDAQ::bufferWord> m_rawData;
  std::vector<VLDAQ::bufferWord> m_clusterAdcBuffer;
  std::vector<VLDAQ::bufferWord> m_clusterPosBuffer;

  /// Size of raw bank in bytes, including the 4 byte header but
  /// *without* the padding bytes at the end
  unsigned int m_bankSizeInBytes;
  /// Version number of the raw data format
  unsigned int m_bankVersion;
  /// Pointer to the detector element
  DeVL* m_det;
  
  /// List of sensor numbers to check for empty sensors
  std::vector<unsigned int> m_sensorNumbers;

  /// Output level
  bool m_debug;
  bool m_verbose;

};
#endif 
