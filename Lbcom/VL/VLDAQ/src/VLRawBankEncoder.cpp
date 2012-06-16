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
    m_bankVersion(1),
    m_det(0) {

}

//=============================================================================
/// Initialisation. Check parameters
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
  // Get the clusters from the input container.
  if (!exist<VLClusters>(VLClusterLocation::Default)) {
    error() << "Cannot retrieve VLClusters from " 
            << VLClusterLocation::Default << endmsg;
    return StatusCode::FAILURE;
  } 
  VLClusters* clusters = get<VLClusters>(VLClusterLocation::Default);
  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(), clusters->end(), m_sortedClusters.begin());
  // Sort the clusters by sensor and strip number.
  std::sort(m_sortedClusters.begin(), m_sortedClusters.end(), 
            VLDataFunctor::LessByStrip<const VLCluster*>());
  RawEvent* rawEvent = getOrCreate<RawEvent, RawEvent>(RawEventLocation::Default);

  /// Loop over all clusters and write one bank per sensor.
  VLClusters::const_iterator first = m_sortedClusters.begin();
  VLClusters::const_iterator last = first;
  unsigned int sensorNumber; 
  // Index of current sensor in the list
  unsigned int sensorIndex = 0; 
  while (first != m_sortedClusters.end()) {
    sensorNumber = (*first)->channelID().sensor();
    // Check for missing sensor numbers.
    while (sensorNumber != m_sensorNumbers[sensorIndex]) {
      // Store an empty bank.
      storeBank(m_sensorNumbers[sensorIndex], last, last, rawEvent); 
      if (m_debug) {
        debug() << "Added empty bank for sensor " 
                << m_sensorNumbers[sensorIndex] << endmsg;
      }
      // Move to next on list and try again.
      ++sensorIndex;
    }
    while (last != m_sortedClusters.end() && 
           (*last)->channelID().sensor() == sensorNumber) {
      ++last;
    }
    // Make and store the bank.
    storeBank(sensorNumber, first, last, rawEvent);
    first = last;
    // Move to next on list of expected sensors.
    ++sensorIndex; 
  }
  // Add any empty banks from final sensor in the list.
  while (sensorIndex < m_sensorNumbers.size()) {
    // Store an empty bank.
    storeBank(m_sensorNumbers[sensorIndex], last, last, rawEvent); 
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


void VLRawBankEncoder::storeBank(int sensor,
                                 VLClusters::const_iterator begin, 
                                 VLClusters::const_iterator end,
                                 RawEvent* rawEvent) {

  // Create raw bank in raw data cache.
  makeBank(begin, end);
  RawBank* newBank = rawEvent->createBank(static_cast<VLDAQ::bufferWord>(sensor),
                                          RawBank::VL,
                                          m_bankVersion,
                                          m_bankSizeInBytes, 
                                          &(m_rawData[0]));
  // Add new bank and pass memory ownership to raw event.
  rawEvent->adoptBank(newBank, true);

}

unsigned int VLRawBankEncoder::makeBank(VLClusters::const_iterator begin,
                                        VLClusters::const_iterator end) {


  if (m_debug) debug() << " ==> makeBank()" << endmsg;
  // Clear bank
  m_rawData.clear();

  // Header
  //   number of clusters on this sensor (first 16 bits)
  //   PCN (next 8 bits), set to zero here (no errors in simulation)
  VLHeaderWord hw(end - begin, 0, 0); 
  VLDAQ::bufferWord pcnAndNumClu = hw.value();

  // Clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterAdcBuffer.clear();
  // 32 bit 'rows' of adc values and cluster positions
  VLDAQ::bufferWord rowData = 0x0; 
  VLDAQ::bufferWord cluRowData = 0x0;

  // Cluster counter
  unsigned int nClu = 0;  
  // ADC word counter (to determine number of padding bytes at end of raw bank)
  unsigned int nAdc = 0;  
  // Loop over clusters in range defined by iterators
  VLClusters::const_iterator it;
  for (it = begin; it != end; ++it) {
    const VLCluster* clu = *it;
    const unsigned int nStrips = clu->size();
    const bool highThreshold = clu->highThreshold();
    if (m_verbose) {
      verbose() << "Reading cluster from sensor "
                << clu->channelID().sensor() << " with " 
                << nStrips << " strips." << endmsg;
    }

    VLDAQ::bufferWord packedCluster;
    if (1 == nStrips) {
      // Single strip cluster
      unsigned int adc = clu->adcValue(0);
      if (adc > 127) adc = 127;
      VLClusterWord vcw(clu->strip(0), 0., 1, highThreshold);
      packedCluster = static_cast<VLDAQ::bufferWord>(vcw.value());
      if (m_verbose) {
        verbose() << "Strip: " << clu->strip(0) 
                  << ", ADC: " << adc << endmsg;
      }
      const bool endOfCluster = true;
      VLADCWord aw(adc, endOfCluster);
      rowData |= (aw.value() << ((nAdc % VLDAQ::adcsPerBuffer) 
                             << VLDAQ::adcShift));
      ++nAdc;
      if (0 == nAdc % VLDAQ::adcsPerBuffer) {
        m_clusterAdcBuffer.push_back(rowData);
        // Reset rowData
        rowData = 0x0;
      }
    } else {
      // Multiple strip cluster. Loop over strips.
      for (unsigned int i = 0; i < clu->size(); ++i) {
        unsigned int adc = clu->adcValue(i);
        if (m_verbose) {
          verbose() << "Strip: " << clu->strip(i) 
                    << ", ADC: " << adc << endmsg;
        }
        const bool endOfCluster = (clu->size() == i + 1);
        // Create new ADC word
        VLADCWord aw(adc, endOfCluster);
        rowData |= (aw.value() << ((nAdc % VLDAQ::adcsPerBuffer) 
                               << VLDAQ::adcShift));
        ++nAdc;
        if (nAdc % VLDAQ::adcsPerBuffer == 0) {
          m_clusterAdcBuffer.push_back(rowData);
          // Reset rowData
          rowData = 0x0;
        }
      } 
      unsigned int channel = clu->strip(0);
      double fraction = clu->interStripFraction();
      VLClusterWord vcw(channel, fraction, nStrips, highThreshold);
      packedCluster = static_cast<VLDAQ::bufferWord>(vcw.value());
      if (m_verbose) {
        verbose() << "Cluster word "  
                  << format("%8X", packedCluster) << endmsg;
      }
    }

    // Store the cluster position
    cluRowData |= (packedCluster << ((nClu % VLDAQ::clustersPerBuffer) 
                                 << VLDAQ::clusterShift));
    ++nClu;
    if (nClu % VLDAQ::clustersPerBuffer == 0) {
      m_clusterPosBuffer.push_back(cluRowData);
      // Clear the cluster buffer 
      cluRowData = 0x0; 
    }
  }
  // If there were an odd number of clusters, store the last cluster row.
  if (0 != (nClu % VLDAQ::clustersPerBuffer)) {
    m_clusterPosBuffer.push_back(cluRowData);
  }
  // Check row data is not empty, store remaining adcs.
  if (0 != (nAdc % VLDAQ::adcsPerBuffer)) {
    m_clusterAdcBuffer.push_back(rowData);
  }

  if (m_verbose) {
    verbose() << "Raw output of cluster start" << endmsg;
    verbose() << "PCN error flag and number of clusters [" 
              << format("%10X", pcnAndNumClu) << "]"<< endmsg;
  }
  // Add header 
  m_rawData.push_back(pcnAndNumClu);
  // Add cluster positions  
  std::vector<VLDAQ::bufferWord>::iterator itc;
  for (itc = m_clusterPosBuffer.begin(); 
       itc != m_clusterPosBuffer.end(); ++itc) {      
    if (m_verbose) {
      verbose() << "Cluster position[" << format("%10X", *itc) << "]" << endmsg;
    }
    m_rawData.push_back(*itc);
  }
  // Add ADC values 
  std::vector<VLDAQ::bufferWord>::iterator ita;
  for (ita = m_clusterAdcBuffer.begin(); 
       ita != m_clusterAdcBuffer.end(); ++ita) {
    if (m_verbose) {
      verbose() << "Adc row[" << format("%10X", *ita) << "]" << endmsg;
    }
    m_rawData.push_back(*ita);
  }

  // Find number of padding bytes at the end of bank an compute
  // raw bank size in bytes, including the 4 byte header but
  // *without* the padding bytes at the end. 
  // The number of padding bytes is completely determined by
  // the number of ADC words in the raw bank.
  int adcRemainder = nAdc % VLDAQ::adcsPerBuffer;
  m_bankSizeInBytes = sizeof(VLDAQ::bufferWord) * m_rawData.size();
  if (adcRemainder) {
    m_bankSizeInBytes -= sizeof(VLDAQ::bufferWord) - 
                         adcRemainder * VLDAQ::adcWordSize;
  }
  return nClu;

}

