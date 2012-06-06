// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel/LHCbKernel
#include "Kernel/VeloLiteDataFunctor.h"
// Event/DAQEvent
#include "Event/RawEvent.h"
// Det/VeloLiteDet
#include "VeloLiteDet/DeVeloLite.h"
// Si/SiDAQ
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiClusterWord.h"
#include "SiDAQ/SiADCWord.h"
// Local
#include "VeloLiteClusterWord.h"
#include "VeloLiteRawWordSizes.h"
#include "PrepareVeloLiteRawBank.h"

DECLARE_ALGORITHM_FACTORY(PrepareVeloLiteRawBank);

using namespace LHCb;

//=============================================================================
/// Constructor
//=============================================================================
PrepareVeloLiteRawBank::PrepareVeloLiteRawBank(const std::string& name,
                                               ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_bankVersion(1),
    m_det(0) {

}

//=============================================================================
/// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareVeloLiteRawBank::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); 
  if (sc.isFailure()) return sc;

  debug() << " ==> initialize()" << endmsg;
  m_debug = msgLevel(MSG::DEBUG);
  m_verbose = msgLevel(MSG::VERBOSE);
  m_det = getDet<DeVeloLite>(DeVeloLiteLocation::Default);
  // Get a list of sensor numbers to identify empty sensors.
  std::vector<DeVeloLiteSensor*>::const_iterator it;
  for (it = m_det->sensorsBegin(); it != m_det->sensorsEnd(); ++it) {
    m_sensorNumbers.push_back((*it)->sensorNumber());
  }
  std::sort(m_sensorNumbers.begin(), m_sensorNumbers.end());
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode PrepareVeloLiteRawBank::execute() {

  if (m_debug) debug() << " ==> execute()" << endmsg;
  // Get the clusters from the input container.
  if (!exist<VeloStripClusters>(VeloStripClusterLocation::Default)) {
    return Error("There are no VeloStripClusters in the TES!");
  } 
  VeloStripClusters* clusters = get<VeloStripClusters>(VeloStripClusterLocation::Default);
  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(), clusters->end(), m_sortedClusters.begin());
  // Sort the clusters by sensor and strip number.
  std::sort(m_sortedClusters.begin(), m_sortedClusters.end(), 
            VeloLiteDataFunctor::Less_by_strip<const VeloStripCluster*>());

  RawEvent* rawEvent;
  // See whether the raw event exists.
  if (exist<RawEvent>(RawEventLocation::Default)) {
    rawEvent = get<RawEvent>(RawEventLocation::Default);
  } else {
    // Raw event doesn't exist. We need to create it.
    rawEvent = new RawEvent();
    eventSvc()->registerObject(RawEventLocation::Default, rawEvent);
  } 

  /// Loop over all clusters and write one bank per sensor.
  VeloStripClusters::const_iterator first = m_sortedClusters.begin();
  VeloStripClusters::const_iterator last = first;
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
StatusCode PrepareVeloLiteRawBank::finalize() {

  return GaudiAlgorithm::finalize();

}


void PrepareVeloLiteRawBank::storeBank(int sensor,
                                       VeloStripClusters::const_iterator begin, 
                                       VeloStripClusters::const_iterator end,
                                       RawEvent* rawEvent) {

  // Create raw bank in raw data cache.
  makeBank(begin, end);
  // Check if the sensor has a TELL40 number.
  unsigned int tellId;
  if (!m_det->tell40IdBySensorNumber(sensor, tellId)) {
    warning() << "Sensor " << sensor 
              << " has no TELL40 ID. No raw bank created." << endmsg; 
    return;
  }
  if (m_debug) {
    debug() << "Sensor " << sensor << ", TELL40 ID " << tellId << endmsg;
  }
  RawBank* newBank = rawEvent->createBank(static_cast<SiDAQ::buffer_word>(tellId),
                                          RawBank::VeloLite,
                                          m_bankVersion,
                                          m_bankSizeInBytes, 
                                          &(m_rawData[0]));
  // Add new bank and pass memory ownership to raw event.
  rawEvent->adoptBank(newBank, true);

}

unsigned int PrepareVeloLiteRawBank::makeBank(VeloStripClusters::const_iterator begin,
                                              VeloStripClusters::const_iterator end) {


  if (m_debug) debug() << " ==> makeBank()" << endmsg;
  // Clear bank
  m_rawData.clear();

  // Header
  //   number of clusters on this sensor (first 16 bits)
  //   PCN (next 8 bits), set to zero here (no errors in simulation)
  SiHeaderWord hw(end - begin, 0, 0); 
  SiDAQ::buffer_word pcnAndNumClu = hw.value();

  // Clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterADCBuffer.clear();
  // 32 bit 'rows' of adc values and cluster positions
  SiDAQ::buffer_word rowData = 0x0; 
  SiDAQ::buffer_word cluRowData = 0x0;

  // Cluster counter
  unsigned int nClu = 0;  
  // ADC word counter (to determine number of padding bytes at end of raw bank)
  unsigned int nAdc = 0;  
  // Loop over clusters in range defined by iterators 
  for (VeloStripClusters::const_iterator it = begin; it != end; ++it) {
    const VeloStripCluster* clu = *it;
    const unsigned int nStrips = clu->size();
    const bool highThreshold = clu->highThreshold();
    if (m_verbose) {
      verbose() << "Reading cluster from sensor "
                << clu->channelID().sensor() << " with " 
                << nStrips << " strips." << endmsg;
    }

    SiDAQ::buffer_word packedCluster;
    if (1 == nStrips) {
      // Single strip cluster
      unsigned int adc = clu->adcValue(0);
      if (adc > 127) adc = 127;
      VeloLiteClusterWord vcw(clu->strip(0), 0., 1, highThreshold);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());
      if (m_verbose) {
        verbose() << "Strip: " << clu->strip(0) 
                  << ", ADC: " << adc << endmsg;
      }
      const bool endOfCluster = true;
      SiADCWord aw(adc, endOfCluster);
      rowData |= (aw.value() << ((nAdc % VeloLiteDAQ::adc_per_buffer) 
                             << VeloLiteDAQ::adc_shift));
      ++nAdc;
      if (nAdc % VeloLiteDAQ::adc_per_buffer == 0) {
        m_clusterADCBuffer.push_back(rowData);
        // Reset rowData
        rowData = 0x0;
      }
    } else {
      // Multiple strip cluster. Loop over strips.
      for (unsigned int i = 0; i < clu->size(); ++i) {
        unsigned int adc = clu->adcValue(i);
        if (adc > 127) adc = 127;
        if (m_verbose) {
          verbose() << "Strip: " << clu->strip(i) 
                    << ", ADC: " << adc << endmsg;
        }
        const bool endOfCluster = (clu->size() == i + 1);
        // Create new ADC word
        SiADCWord aw(adc, endOfCluster);
        rowData |= (aw.value() << ((nAdc % VeloLiteDAQ::adc_per_buffer) 
                               << VeloLiteDAQ::adc_shift));
        ++nAdc;
        if (nAdc % VeloLiteDAQ::adc_per_buffer == 0) {
          m_clusterADCBuffer.push_back(rowData);
          // Reset rowData
          rowData = 0x0;
        }
      } 
      unsigned int channel = clu->strip(0);
      double fraction = clu->interStripFraction();
      VeloLiteClusterWord vcw(channel, fraction, nStrips, highThreshold);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());
      if (m_verbose) {
        verbose() << "Cluster word "  
                  << format("%8X", packedCluster) << endmsg;
      }
    }

    // Store the cluster position
    cluRowData |= (packedCluster << ((nClu % VeloLiteDAQ::clu_per_buffer) 
                                 << VeloLiteDAQ::clu_shift));
    ++nClu;
    if (nClu % VeloLiteDAQ::clu_per_buffer == 0) {
      m_clusterPosBuffer.push_back(cluRowData);
      // Clear the cluster buffer 
      cluRowData = 0x0; 
    }
  }
  // If there were an odd number of clusters, store the last cluster row
  if (0 != (nClu % VeloLiteDAQ::clu_per_buffer)) {
    m_clusterPosBuffer.push_back(cluRowData);
  }
  // Check row data is not empty, store remaining adcs
  if (0 != (nAdc % VeloLiteDAQ::adc_per_buffer)) {
    m_clusterADCBuffer.push_back(rowData);
  }

  if (m_verbose) {
    verbose() << "Raw output of cluster start" << endmsg;
    verbose() << "PCN error flag and number of clusters [" 
              << format("%10X", pcnAndNumClu) << "]"<< endmsg;
  }
  // Add header 
  m_rawData.push_back(pcnAndNumClu);
  // Add cluster positions  
  std::vector<SiDAQ::buffer_word>::iterator itc;
  for (itc = m_clusterPosBuffer.begin(); 
       itc != m_clusterPosBuffer.end(); ++itc) {      
    if (m_verbose) {
      verbose() << "Cluster position[" << format("%10X", *itc) << "]" << endmsg;
    }
    m_rawData.push_back(*itc);
  }
  // Add ADC values 
  std::vector<SiDAQ::buffer_word>::iterator ita;
  for (ita = m_clusterADCBuffer.begin(); 
       ita != m_clusterADCBuffer.end(); ++ita) {
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
  int adcRemainder = nAdc % VeloLiteDAQ::adc_per_buffer;
  m_bankSizeInBytes = sizeof(SiDAQ::buffer_word) * m_rawData.size();
  if (adcRemainder) {
    m_bankSizeInBytes -= sizeof(SiDAQ::buffer_word) - 
                         adcRemainder * VeloLiteDAQ::adc_word_size;
  }
  return nClu;

}

