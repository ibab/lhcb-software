// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
// Kernel/LHCbKernel
#include "Kernel/VLDataFunctor.h"
// Event/DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
// Event/DigiEvent
#include "Event/VLLiteCluster.h"
#include "Event/VLCluster.h"
#include "Event/ProcStatus.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "VLHeaderWord.h"
#include "VLClusterWord.h"
#include "VLADCWord.h"
#include "VLRawBankDecoder.h"

using namespace LHCb;

/** @file VLRawBankDecoder.cpp
 *
 *  Implementation of class : VLRawBankDecoder
 *
 */

DECLARE_ALGORITHM_FACTORY(VLRawBankDecoder)

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
VLRawBankDecoder::VLRawBankDecoder(const std::string& name,
                                   ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_liteClusters(0),
    m_clusters(0),
    m_det(0) {
    
  declareProperty("DecodeToLiteClusters", m_decodeToLiteClusters = true);
  declareProperty("DecodeToClusters",     m_decodeToClusters = false);

  declareProperty("RawEventLocation", 
                  m_rawEventLocation = RawEventLocation::Default);
  declareProperty("LiteClusterLocation",
                  m_liteClusterLocation = VLLiteClusterLocation::Default);
  declareProperty("ClusterLocation",
                  m_clusterLocation = VLClusterLocation::Default);

}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode VLRawBankDecoder::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << " ==> initialise()" << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VLRawBankDecoder::execute() {

  if (m_decodeToLiteClusters) {
    m_liteClusters = new VLLiteCluster::VLLiteClusters();
    m_liteClusters->reserve(4096); 
  }
  if (m_decodeToClusters) {
    m_clusters = new VLClusters();
  }
  // Get the raw event.
  if (!exist<RawEvent>(m_rawEventLocation)) {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "No raw event in " << m_rawEventLocation << endmsg;
    }
    // Store empty containers.
    if (m_decodeToLiteClusters) {
      put(m_liteClusters, m_liteClusterLocation);
    }
    if (m_decodeToClusters) {
      put(m_clusters, m_clusterLocation);
    }
    return StatusCode::SUCCESS;
  } 
  RawEvent* rawEvent = get<RawEvent>(m_rawEventLocation);
  // Get the VL raw banks.
  const std::vector<RawBank*>& banks = rawEvent->banks(RawBank::VL);
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Retrieved " << banks.size() << " VL raw banks" << endmsg;
  }
  std::vector<RawBank*>::const_iterator it;
  for (it = banks.begin(); it != banks.end(); ++it) {
    // Do the decoding.
    decode(*it);
  }
  if (m_decodeToLiteClusters) {
    std::sort(m_liteClusters->begin(), m_liteClusters->end(),
              VLDataFunctor::LessByChannel<VLLiteCluster>());
    put(m_liteClusters, m_liteClusterLocation);
  }
  if (m_decodeToClusters) {
    put(m_clusters, m_clusterLocation);
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Final processing
//=============================================================================
StatusCode VLRawBankDecoder::finalize() {

  return GaudiAlgorithm::finalize();

}


//=============================================================================
/// Decode raw data format to clusters and lite clusters 
//=============================================================================
void VLRawBankDecoder::decode(const RawBank* rb) {

  ProcStatus* pst;
  // Protect against corrupted banks.
  if (RawBank::MagicPattern != rb->magic()) {
    // Set ProcStat for this event to failed.
    pst = getOrCreate<ProcStatus, ProcStatus>(ProcStatusLocation::Default);
    pst->addAlgorithmStatus("VLRawBankDecoder", "VL", 
                            "CorruptBuffer", CorruptBuffer, false);
    error() << "Bad magic pattern (source ID " 
            << rb->sourceID() << ")" << endmsg;
    return;
  }
  // Check if the source ID corresponds to a valid sensor number.
  const unsigned int sensorNumber = static_cast<unsigned int>(rb->sourceID());
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Decoding raw bank for sensor " << sensorNumber << endmsg;
  }
  const DeVLSensor* sensor = m_det->sensor(sensorNumber);
  if (!sensor) {
    // Set ProcStat for this event to failed.
    pst = getOrCreate<ProcStatus, ProcStatus>(ProcStatusLocation::Default);
    pst->addAlgorithmStatus("VLRawBankDecoder", "VL", 
                            "BadSensorNumber", BadSensorNumber, false);
    error() << "Invalid sensor number (source ID "
            << rb->sourceID() << ")" << endmsg;
    return;
  }
  const VLDAQ::row* data = static_cast<const VLDAQ::row*>(rb->data());
  VLHeaderWord hw(data[0]);
  // Check if the error bit in the header has been set.
  if (hw.hasError()) {
    // Set ProcStat for this event to failed.
    pst = getOrCreate<ProcStatus, ProcStatus>(ProcStatusLocation::Default);
    pst->addAlgorithmStatus("VLRawBankDecoder", "VL", 
                            "HeaderErrorBit", HeaderErrorBit, false);
    error() << "Header error bit set (source ID "
            << rb->sourceID() << ")" << endmsg;
    return;
  }
  unsigned int nClusters = hw.nClusters();
  if (msgLevel(MSG::DEBUG)) {
    debug() << nClusters << " clusters" << endmsg;
  }
  const unsigned int offset = 1 + nClusters / 2 + nClusters % 2;
  VLDAQ::row adcrow = data[offset];
  // Keep track of the number of ADC words read.
  unsigned int nADCs = 0;
  for (unsigned int i = 0; i < nClusters; ++i) {
    // Get the cluster word.
    VLClusterWord cw((data[1 + i / 2] >> ((i % 2) << 4)) & 0x0000FFFF);
    // Decode the cluster word.
    const unsigned int stripNumber = cw.channel();
    const double isp = cw.interStripPosition();
    const bool high = cw.highAdc();
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Cluster " << i << ": strip " << stripNumber 
              << ", inter-strip fraction " << isp << endmsg;
    }
    // Set the channel ID.
    VLChannelID channelID(sensorNumber, stripNumber, VLChannelID::Null);
    if (sensor->isPhi()) {
      channelID.setType(VLChannelID::PhiType);
    } else {
      channelID.setType(VLChannelID::RType);
    }
    // Create a lite cluster.
    // Set the pseudo-size to 1 for now.
    VLLiteCluster liteCluster(channelID, isp, high);
    m_liteClusters->push_back(liteCluster);
    if (!m_decodeToClusters) continue;
    std::vector<std::pair<int, unsigned int> > strips;
    // Get the ADC words.
    bool endCluster = false;
    unsigned int nStrips = 0;
    unsigned int firstStrip = 0;
    while (!endCluster) {
      // After having read 4 ADC words, move to a new row.
      if (0 == nADCs % 4) adcrow = data[offset + nADCs / 4];
      VLADCWord aw((adcrow >> ((nADCs % 4) << 3)) & 0x000000FF);
      // Decode the ADC word.
      endCluster = aw.endCluster();
      // Check if this is the strip used in the lite cluster. 
      if (aw.centralStrip()) firstStrip = stripNumber - nStrips;
      const unsigned int adc = aw.adc();
      strips.push_back(std::make_pair<int, unsigned int>(nStrips, adc)); 
      ++nADCs;
      ++nStrips;
    }
    // Set the strip indices.
    for (unsigned int j = 0; j < nStrips; ++j) {
      strips[j].first = firstStrip + j;
    }
    // Create a cluster.
    VLCluster* cluster = new VLCluster(liteCluster, strips);
    m_clusters->insert(cluster, channelID);
  }

}
