#ifndef PACKEDDATADECODER_H
#define PACKEDDATADECODER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include "HltRawBankDecoderBase.h"
#include "PackedDataChecksum.h"

/** @class HltPackedDataDecoder HltPackedDataDecoder.h
 *  Algorithm that reads packed objects from raw banks.
 *
 *  @author Rosen Matev, Sean Benson
 *  @date   2016-01-03
 */
class HltPackedDataDecoder: public HltRawBankDecoderBase {
public:

  enum HeaderIDs { kVersionNumber = 2 };
  
  /// Standard constructor
  HltPackedDataDecoder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  ~HltPackedDataDecoder( ) override = default;
  
  StatusCode initialize() override; ///< Algorithm initialization
  StatusCode execute() override; ///< Algorithm execution
  StatusCode finalize() override; ///< Algorithm finalization
  
private:
  /// Load an object from the buffer and save it to TES.
  template<typename T> size_t loadObject(const std::string& location);
  
  /// Map between CLIDs and load functions
  std::map<CLID, std::function<size_t(const std::string&)> > m_loaders;
  /// Buffer for de-serialization of the packed objects
  PackedDataPersistence::PackedDataInBuffer m_buffer; 
  /// Helper for computing checksums
  PackedDataPersistence::PackedDataChecksum* m_checksum{nullptr};
};

#endif // PACKEDDATADECODER_H
