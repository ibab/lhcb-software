#include "RZip.h"

#include "Event/RawEvent.h"
#include "Event/PackedTrack.h"
#include "Event/PackedCaloCluster.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedCaloHypo.h"
#include "Event/PackedProtoParticle.h"

#include "HltPackedDataWriter.h"
#include "HltPackedDataDecoder.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(HltPackedDataDecoder)


HltPackedDataDecoder::HltPackedDataDecoder(const std::string& name,
                                           ISvcLocator* pSvcLocator)
  : HltRawBankDecoderBase(name, pSvcLocator)
{
  declareProperty("EnableChecksum", m_enableChecksum = false);
  
  // The default m_sourceID=0 triggers a warning in HltRawBankDecoderBase::initialize
  // Since we only care about HLT2 persistence, set it explicitly:
  m_sourceID = kSourceID_Hlt2;

  using namespace std::placeholders; 
  m_loaders[LHCb::CLID_PackedTracks] =
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedTracks>, this, _1);
  m_loaders[LHCb::CLID_PackedRichPIDs] = 
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedRichPIDs>, this, _1);
  m_loaders[LHCb::CLID_PackedMuonPIDs] = 
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedMuonPIDs>, this, _1);
  m_loaders[LHCb::CLID_PackedCaloHypos] = 
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedCaloHypos>, this, _1);
  m_loaders[LHCb::CLID_PackedProtoParticles] = 
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedProtoParticles>, this, _1);
  m_loaders[LHCb::CLID_PackedCaloClusters] = 
    std::bind(&HltPackedDataDecoder::loadObject<LHCb::PackedCaloClusters>, this, _1);
}


StatusCode HltPackedDataDecoder::initialize() {
  const StatusCode sc = HltRawBankDecoderBase::initialize();
  if (sc.isFailure()) return sc;

  if (UNLIKELY(m_enableChecksum)) {
    m_checksum = new PackedDataPersistence::PackedDataChecksum();
  }

  return StatusCode::SUCCESS;
}


template<typename T>
std::pair<DataObject*, size_t> HltPackedDataDecoder::loadObject(const std::string& location) {
  auto object = new T{};
  put(object, location);
  auto nBytesRead = m_buffer.load(*object);
  if (UNLIKELY(m_enableChecksum)) m_checksum->processObject(*object);
  return std::make_pair(dynamic_cast<DataObject*>(object), nBytesRead);
}


StatusCode HltPackedDataDecoder::execute() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  auto* rawEvent = findFirstRawEvent();
  if (!rawEvent) {
    return Error("Raw event not found!");
  }

  const auto& rawBanksConst = rawEvent->banks(LHCb::RawBank::DstData);
  if (rawBanksConst.empty()) {
    return Warning("No appropriate HltPackedData raw bank in raw event. Quitting.",
                   StatusCode::SUCCESS, 10);
  }

  std::vector<const LHCb::RawBank*> rawBanks;
  rawBanks.reserve(rawBanksConst.size());
  std::copy(std::begin(rawBanksConst), std::end(rawBanksConst),
            std::back_inserter(rawBanks));

  const auto* rawBank0 = *(rawBanks.begin());

  // Check we know how to decode this version
  if (rawBank0->version() < 2 || rawBank0->version() > kVersionNumber) {
    return Error("HltPackedData raw bank version is not supported.");
  }

  // Put the banks into the right order
  std::sort(
    std::begin(rawBanks), std::end(rawBanks), 
    [](const LHCb::RawBank* lhs, const LHCb::RawBank* rhs) { 
      auto l = HltPackedDataWriter::partID(lhs->sourceID());
      auto r = HltPackedDataWriter::partID(rhs->sourceID());
      return l < r; 
    });

  // Check that the banks have sequential part IDs
  unsigned int payloadSize = 0;
  for (unsigned int i = 0; i < rawBanks.size(); ++i) {
    if (HltPackedDataWriter::partID(rawBanks[i]->sourceID()) != i)
      return Error("Part IDs for HltPackedData banks are not sequential. Quitting.", StatusCode::SUCCESS, 20);
    payloadSize += rawBanks[i]->size();
  }

  // Collect the data into a contiguous buffer
  std::vector<uint8_t> payload;
  payload.reserve(payloadSize);
  for(const auto* bank: rawBanks) {
    std::copy(bank->begin<uint8_t>(), bank->end<uint8_t>(), std::back_inserter(payload));
  }

  auto compression = HltPackedDataWriter::compression(rawBank0->sourceID());
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Compression " << compression << ", payload size " << payload.size() << endmsg;
  }

  // Decompress the payload and load into the buffer
  switch (compression) {
    case HltPackedDataWriter::NoCompression: {
      m_buffer.init(payload);
      break;
    }
    case HltPackedDataWriter::ZLIB:
    case HltPackedDataWriter::LZMA: {
      if (!m_buffer.init(payload, true))
        return Error("Failed to decompress HltPackedData. Quitting.");
      break;
    }
    default: {
      return Error("Unknown compression method. Quitting.", StatusCode::SUCCESS, 20);
      break;
    }
  }

  // Get the map of ids to locations (may differ between events)
  const auto& locationsMap = packedObjectLocation2string(tck());

  std::vector<int32_t> linkLocationIDs;

  // Do the actual loading of the objects
  while (!m_buffer.eof()) {
    // Load the metadata
    CLID classID = m_buffer.load<uint32_t>();
    auto locationID = m_buffer.load<int32_t>();
    m_buffer.load(linkLocationIDs);
    auto storedObjectSize = m_buffer.load<uint32_t>();

    auto locationIt = locationsMap.find(locationID);
    if (locationIt == std::end(locationsMap)) {
      error() << "Packed object location not found in ANNSvc for id=" << locationID
              << ". Skipping reading the container!" << endmsg;
      m_buffer.skip(storedObjectSize);
      continue;
    }
    std::string containerPath = locationIt->second;
    
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Reading " << storedObjectSize << " bytes "
              << "for object with CLID " << classID << " into TES location "
              << containerPath << endmsg;
    }

    const auto it = m_loaders.find(classID);
    if (it == m_loaders.end()) {
      error() << "Unknown class ID " << classID << endmsg;
      m_buffer.skip(storedObjectSize);
      continue;
    }
    const auto loadObjectFun = it->second;

    // Load the packed object
    auto ret = loadObjectFun(containerPath);
    auto dataObject = ret.first;
    auto readObjectSize = ret.second;

    if (readObjectSize != storedObjectSize) {
      fatal() << "Loading of object (CLID=" << classID << ") "
              << "to " << containerPath << " "
              << "consumed " << readObjectSize << " bytes, "
              << "but " << storedObjectSize << " were stored!" << endmsg;
      return StatusCode::FAILURE;
    }

    // Restore the links to other containers on the TES
    for (const auto& linkLocationID: linkLocationIDs) {
      auto locationIt = locationsMap.find(linkLocationID);
      if (locationIt == std::end(locationsMap)) {
        error() << "Packed object location not found in ANNSvc for id=" << linkLocationID
                << ". Skipping this link, unpacking may fail!" << endmsg;
        continue;
      }
      dataObject->linkMgr()->addLink(locationIt->second, nullptr);
    }
  }

  if (UNLIKELY(m_enableChecksum)) {
    debug() << "packed data checksum = " << m_checksum->checksum() << endmsg;
  }

  m_buffer.clear();
  // TODO do we / can we shrink the buffer if it takes too much space for an event

  return StatusCode::SUCCESS;
}


StatusCode HltPackedDataDecoder::finalize() {
  if (UNLIKELY(m_enableChecksum)) {
    info() << "Global packed data checksum = " << m_checksum->checksum() << endmsg;
    delete m_checksum;
  }
  return HltRawBankDecoderBase::finalize();  // must be called after all other actions
}