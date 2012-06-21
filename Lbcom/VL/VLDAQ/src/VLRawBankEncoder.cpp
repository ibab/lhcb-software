// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel/LHCbKernel
#include "Kernel/VLDataFunctor.h"
// Event/DAQEvent
#include "Event/RawEvent.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "VLHeaderWord.h"
#include "VLClusterWord.h"
#include "VLADCWord.h"
#include "VLRawBankConstants.h"
#include "VLRawBankEncoder.h"

DECLARE_ALGORITHM_FACTORY(VLRawBankEncoder)

using namespace LHCb;

//=============================================================================
/// Constructor
//=============================================================================
VLRawBankEncoder::VLRawBankEncoder(const std::string& name,
                                   ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_clusters(0),
    m_bankVersion(1),
    m_det(0) {

}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode VLRawBankEncoder::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); 
  if (sc.isFailure()) return sc;
  m_debug   = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);
  if (m_debug) debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  // Get a list of sensor numbers to identify empty sensors.
  std::vector<DeVLSensor*>::const_iterator it;
  for (it = m_det->sensorsBegin(); it != m_det->sensorsEnd(); ++it) {
    m_sensorNumbers.push_back((*it)->sensorNumber());
  }
  std::sort(m_sensorNumbers.begin(), m_sensorNumbers.end());
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VLRawBankEncoder::execute() {

  if (m_debug) debug() << " ==> execute()" << endmsg;
  // Get the clusters.
  if (!exist<VLClusters>(VLClusterLocation::Default)) {
    error() << "Cannot retrieve VLClusters from " 
            << VLClusterLocation::Default << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusters = get<VLClusters>(VLClusterLocation::Default);
  // Sort the clusters by ascending sensor and strip number.
  std::sort(m_clusters->begin(), m_clusters->end(), 
            VLDataFunctor::LessByStrip<const VLCluster*>());
            
  // Get the raw event.
  RawEvent* rawEvent = getOrCreate<RawEvent, RawEvent>(RawEventLocation::Default);

  // Loop over the clusters and write one bank per sensor.
  VLClusters::const_iterator it = m_clusters->begin();
  unsigned int sensorIndex = 0; 
  while (it != m_clusters->end()) {
    unsigned int sensorNumber = (*it)->channelID().sensor();
    // Check for missing sensor numbers.
    while (sensorNumber != m_sensorNumbers[sensorIndex]) {
      // Create an empty bank.
      encode(m_sensorNumbers[sensorIndex], it, it, rawEvent); 
      if (m_debug) {
        debug() << "Added empty bank for sensor " 
                << m_sensorNumbers[sensorIndex] << endmsg;
      }
      // Move to the next sensor on the list and try again.
      ++sensorIndex;
    }
    // Get the last cluster on this sensor.
    VLClusters::const_iterator last = it;
    while (last != m_clusters->end() && 
           (*last)->channelID().sensor() == sensorNumber) {
      ++last;
    }
    // Create a bank for this sensor.
    encode(sensorNumber, it, last, rawEvent);
    // Move to the next sensor.
    it = last;
    ++sensorIndex; 
  }
  // Create empty banks for remaining sensors (if any).
  while (sensorIndex < m_sensorNumbers.size()) {
    encode(m_sensorNumbers[sensorIndex], it, it, rawEvent); 
    if (m_debug) {
      debug() << "Added empty bank for sensor " 
              << m_sensorNumbers[sensorIndex] << endmsg;
    }
    ++sensorIndex;
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Finalisation 
//=============================================================================
StatusCode VLRawBankEncoder::finalize() {

  return GaudiAlgorithm::finalize();

}


//=============================================================================
/// Encode the clusters on a given sensor in raw data format 
//=============================================================================
void VLRawBankEncoder::encode(int sensor,
                              VLClusters::const_iterator begin, 
                              VLClusters::const_iterator end,
                              RawEvent* rawEvent) {


  if (m_debug) debug() << " ==> encode()" << endmsg;
  // Clear the raw data store.
  m_rawData.clear();

  // Create a header and add it to the raw bank.
  // It comprises the number of clusters on this sensor (16 bits),
  // PCN (8 bits), and an error flag (set to false in the simulation).
  VLHeaderWord hw(end - begin, 0, false); 
  if (m_verbose) {
    verbose() << "Header:" << endmsg;
    verbose() << format("%10X", hw.value()) << endmsg;
  }
  m_rawData.push_back(hw.value());
  
  // Clear the temporary buffers.
  m_clusterRows.clear();
  m_adcRows.clear();
  // 32 bit rows of ADC values and cluster positions
  VLDAQ::row adcRow     = 0x0; 
  VLDAQ::row clusterRow = 0x0;
  // Count the number of cluster and ADC words.
  unsigned int nClusters = 0;  
  unsigned int nADCs = 0;  
  // Loop over the clusters in the specified range.
  VLClusters::const_iterator itc;
  for (itc = begin; itc != end; ++itc) {
    const VLCluster* cluster = *itc;
    const bool high = cluster->highThreshold();
    const unsigned int channel = cluster->channelID().strip();
    const double isp = cluster->interStripFraction();
    // Create a cluster word and add it to the cluster row.
    // It comprises the central channel (12 bits), 
    // the interstrip position (3 bits) and a high ADC flag.
    VLClusterWord cw(channel, isp, high);
    clusterRow |= static_cast<VLDAQ::row>(cw.value()) 
        << ((nClusters % VLDAQ::clustersPerRow) << VLDAQ::clusterShift);
    ++nClusters;
    // Check if the cluster row is full.
    if (0 == nClusters % VLDAQ::clustersPerRow) {
      m_clusterRows.push_back(clusterRow);
      clusterRow = 0x0; 
    }
    // Store the ADC values.
    const unsigned int nStrips = cluster->size();
    for (unsigned int i = 0; i < nStrips; ++i) {
      const unsigned int strip = cluster->strip(i);
      const unsigned int adc = cluster->adcValue(i);
      const bool centralStrip = (strip == channel);
      const bool endOfCluster = (nStrips == i + 1);
      // Create an ADC word and add it to the ADC row.
      VLADCWord aw(adc, centralStrip, endOfCluster);
      adcRow |= (aw.value() 
          << ((nADCs % VLDAQ::adcsPerRow) << VLDAQ::adcShift));
      ++nADCs;
      // Check if the ADC row is full.
      if (0 == nADCs % VLDAQ::adcsPerRow) {
        m_adcRows.push_back(adcRow);
        adcRow = 0x0;
      }
    } 
  }
  // Store remaining cluster words.
  if (0 != (nClusters % VLDAQ::clustersPerRow)) {
    m_clusterRows.push_back(clusterRow);
  }
  // Store remaining ADC words.
  if (0 != (nADCs % VLDAQ::adcsPerRow)) {
    m_adcRows.push_back(adcRow);
  }

  // Add the cluster positions and ADC values to the raw bank.   
  std::vector<VLDAQ::row>::iterator itr;
  if (m_verbose) verbose() << "Cluster positions:" << endmsg;
  for (itr = m_clusterRows.begin(); itr != m_clusterRows.end(); ++itr) { 
    if (m_verbose) verbose() << format("%10X", *itr) << endmsg;
    m_rawData.push_back(*itr);
  }
  if (m_verbose) verbose() << "ADC values:" << endmsg;
  for (itr = m_adcRows.begin(); itr != m_adcRows.end(); ++itr) {
    if (m_verbose) verbose() << format("%10X", *itr) << endmsg;
    m_rawData.push_back(*itr);
  }

  // Compute the raw bank size in bytes.
  unsigned int bankSizeInBytes = sizeof(VLDAQ::row) * m_rawData.size();
  // Subtract the padding bytes at the end.
  const int remainder = nADCs % VLDAQ::adcsPerRow;
  if (remainder > 0) {
    bankSizeInBytes -= sizeof(VLDAQ::row) - remainder * VLDAQ::adcWordSize;
  }

  RawBank* newBank = rawEvent->createBank(static_cast<VLDAQ::row>(sensor), 
                                          RawBank::VL, m_bankVersion,
                                          bankSizeInBytes, &(m_rawData[0]));
  // Add the new bank to the raw event and pass the memory ownership.
  rawEvent->adoptBank(newBank, true);

}
