#ifndef HLTPACKEDDATAWRITER_H
#define HLTPACKEDDATAWRITER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IANNSvc.h"
#include "PackedDataBuffer.h"
#include "PackedDataChecksum.h"

/** @class HltPackedDataWriter HltPackedDataWriter.h
 *  Algorithm that writes packed objects to raw banks.
 *
 *  @author Rosen Matev, Sean Benson
 *  @date   2016-01-03
 */
class HltPackedDataWriter: public GaudiAlgorithm {
public:
  
  enum HeaderIDs{kVersionNumber = 2};

  /// Bit postions in the SourceID word
  enum SourceIDBits {
    PartIDBits = 0,
    ReservedBits = 8,
    CompressionBits = 13
  };
  /// Bit masks in the SourceID word
  enum SourceIDMasks {
    PartIDMask = 0x00FF,
    ReservedMask = 0x1F00,
    CompressionMask = 0xE000
  };
  /// Compression algorithms
  enum Compression {
    NoCompression = 0,
    ZLIB = 1,
    LZMA = 2,
    MaxCompression = 7
  };

  /// Standard constructor
  HltPackedDataWriter(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  ~HltPackedDataWriter( ) override = default;
  
  StatusCode initialize() override; ///< Algorithm initialization
  StatusCode execute() override; ///< Algorithm execution
  StatusCode finalize() override; ///< Algorithm finalization
  
  /// Return the part ID from the SourceID word
  static unsigned int partID(uint16_t sourceID) {
    return (sourceID & PartIDMask) >> PartIDBits;
  }
  /// Return the compression from the SourceID word
  static Compression compression(uint16_t sourceID) {
    return static_cast<Compression>((sourceID & CompressionMask) >> CompressionBits);
  }

  
private:
  /// Put the (compressed) data buffer into raw banks and register them.
  void addBanks(LHCb::RawEvent& rawEvent, const std::vector<uint8_t>& data, Compression compression);
  /// Save an object to the buffer.
  template<typename T> size_t saveObject(const DataObject& dataObject);

  /// Property giving the locations of containers to be persisted
  std::vector<std::string> m_containers; 
  /// Property giving the location of the raw event
  StringProperty m_outputRawEventLocation;
  /// Property setting the compression algorithm
  int m_compression;
  /// Property setting the compression level
  int m_compressionLevel;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc{nullptr};
  
  /// Map between CLIDs and save functions
  std::map<CLID, std::function<size_t(const DataObject&)> > m_savers;
  /// Buffer for serialization of the packed objects
  PackedDataPersistence::PackedDataOutBuffer m_buffer;
  /// Buffer for the compressed data
  std::vector<uint8_t> m_compressedBuffer;
  /// Helper for computing checksums
  PackedDataPersistence::PackedDataChecksum* m_checksum{nullptr};
  /// ROOT compression algorithm
  ROOT::ECompressionAlgorithm m_compressionAlg;

};

#endif // HLTPACKEDDATAWRITER_H
