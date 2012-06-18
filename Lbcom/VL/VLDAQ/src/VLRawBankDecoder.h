#ifndef VL_RAWBANK_DECODER_H 
#define VL_RAWBANK_DECODER_H 1

#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"
#include "VLRawBankConstants.h"

namespace LHCb {
  class RawBank;
}

class DeVL;

/** @class VLRawBankDecoder VLRawBankDecoder.h
 *
 */

class VLRawBankDecoder : public GaudiAlgorithm {
public:
  /// Standard constructor
  VLRawBankDecoder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLRawBankDecoder() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalisation

  enum AlgStatusType { 
    OK              =  0,
    BadSensorNumber =  1,
    CorruptBuffer   =  2,
    HeaderErrorBit  =  3,
    Other           = 99
  };
    
private:

  /// Decode raw bank to clusters and lite clusters. 
  void decode(const std::vector<LHCb::RawBank*>& banks);

  bool m_decodeToLiteClusters;
  bool m_decodeToClusters;

  std::string m_rawEventLocation;
  std::string m_liteClusterLocation;
  std::string m_clusterLocation;

  LHCb::VLLiteCluster::FastContainer* m_liteClusters;
  LHCb::VLClusters* m_clusters;
  const DeVL* m_det;

};
#endif 
