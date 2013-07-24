#ifndef VL_RAWBANK_ENCODER_H 
#define VL_RAWBANK_ENCODER_H 1

// STL
#include <vector>
#include <algorithm>
#include <string>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Local
#include "VLRawBankConstants.h"

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

private:
  
  void encode(int sensor,
              LHCb::VLClusters::const_iterator begin, 
              LHCb::VLClusters::const_iterator end,
              LHCb::RawEvent* rawEvent);

  std::vector<VLDAQ::row> m_rawData;
  std::vector<VLDAQ::row> m_adcRows;
  std::vector<VLDAQ::row> m_clusterRows;

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
