#include "Event/RawEvent.h"

#include "VeloDet/DeVelo.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "SiDAQ/SiRawBufferWord.h"

#include "VeloClusterWord.h"
#include "VeloRawBankVersions.h"
#include "VeloRawBankDecoder.h"
#include "VeloRawWordSizes.h"
#include "VeloClusterPtrLessThan.h"
#include "DecodeRawBankToClusters.h"

#include "VeloClustersToRaw.h"


//-----------------------------------------------------------------------------
// Implementation file for class : VeloClustersToRaw
//
// 2011-09-20 : Kurt Rinnert
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( VeloClustersToRaw )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClustersToRaw::VeloClustersToRaw( const std::string& name,
    ISvcLocator* pSvcLocator) 
: 
  GaudiAlgorithm (name , pSvcLocator),
  m_clusterLoc(LHCb::VeloClusterLocation::Default),
  m_rawEventLoc(LHCb::RawEventLocation::Default),
  m_bankVersion(VeloDAQ::v3),
  m_rawEventOut(NULL),
  m_bankSizeInBytes(0),
  m_velo(NULL)
{
  declareProperty("VeloClusterLocation",m_clusterLoc="Raw/Velo/ClustersSelected");
  declareProperty("RawEventLocation",m_rawEventLoc="DAQ/RawEventSelected");
  declareProperty("RunSelfTest",m_runSelfTest=false,"Decodes endoced bank and compares to input clusters.");
  declareProperty("BankVersion", m_bankVersion=VeloDAQ::v3);
}

//=============================================================================
// Destructor
//=============================================================================
VeloClustersToRaw::~VeloClustersToRaw() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloClustersToRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  // check whether the requested bank version is supported
  switch (m_bankVersion) {
    case VeloDAQ::v2: // ok, do nothing
    case VeloDAQ::v3:
      break; 
    default: // not supported, bail out
      error() << "VELO raw buffer version " 
        << m_bankVersion 
        << " is not supported."
        << endmsg;
      return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloClustersToRaw::execute() {

  StatusCode sc;

  // Get the input container
  // Get the VeloClusters from their default location 
  const LHCb::VeloClusters* clusters = getIfExists<LHCb::VeloClusters>(m_clusterLoc);
  if( NULL == clusters ){
    return Error( " ==> There are no VeloClusters in TES! " );
  }

  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(),clusters->end(),m_sortedClusters.begin());

  // Then sort the clusters by sensor number and strip number
  std::sort( m_sortedClusters.begin(), m_sortedClusters.end(), 
      VeloDAQ::veloClusterPtrLessThan());
  
  m_rawEventOut = getIfExists<LHCb::RawEvent>(m_rawEventLoc);
  if( NULL == m_rawEventOut ) {
    m_rawEventOut = new LHCb::RawEvent();
    put(m_rawEventOut, m_rawEventLoc);
  } 

  // loop over all clusters and write one bank per sensor
  std::vector<const LHCb::VeloCluster*>::const_iterator clusterIter = m_sortedClusters.begin();

  unsigned int currentSensorNumber;  // sensor number for current cluster subset
  //unsigned int sensorIndex = 0; // index of current sensor in list of all sensors

  while (clusterIter != m_sortedClusters.end()) {

    currentSensorNumber = (*clusterIter)->channelID().sensor();

    // make and store the bank, increments cluster iterator to first
    // cluster on next non-empty sensor
    sc = storeBank(currentSensorNumber, clusterIter);
    if ( !sc ) { return StatusCode::FAILURE; } 
  }

  if ( m_runSelfTest ) {
    if ( selfTest() ) {
      info() << "Self test passed." << endmsg;
    } else {
      error() << "Self test FAILED." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode VeloClustersToRaw::storeBank( const unsigned int sensor,
    std::vector<const LHCb::VeloCluster*>::const_iterator& clusterIter) {


  // Sensor and TELL1 Id might be different, e.g in a test beam
  // setup.  Also it makes no sense to add a bank for a sensor that
  // has no Tell1 Id.  While this should never be attempted in the 
  // first place (if the geometry XML and CondDB is correct), it is
  // still a good idea to protect against this scenario. 
  unsigned int sourceId;
  if (m_velo->tell1IdBySensorNumber(sensor, sourceId)) {

    // create new raw buffer in raw data cache, old one is cleared
    makeBank(sensor, clusterIter);

    LHCb::RawBank* newBank = m_rawEventOut->createBank(static_cast<SiDAQ::buffer_word>(sourceId),
        LHCb::RawBank::Velo,
        m_bankVersion,
        m_bankSizeInBytes, 
        &(m_rawData[0]));

    // add new bank and pass memory ownership to raw event
    m_rawEventOut->adoptBank(newBank,true);

    return StatusCode::SUCCESS;

  } else {
    // we should not end up here if the geometry XMl and the
    // CondDB sensor/Tell1 mapping is consistent
    error() 
      << " Attempt to create bank for sensor " << sensor 
      << " with no TELL1 Id detected." << endmsg;

   return StatusCode::FAILURE; 
  }
}

unsigned int VeloClustersToRaw::makeBank ( const unsigned int sensor,
    std::vector<const LHCb::VeloCluster*>::const_iterator& clusterIter ) 
{
  // clear bank
  m_rawData.clear();

  // clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterADCBuffer.clear();

  // for storing 32 bit 'rows' of adc values and compressed clusters 
  SiDAQ::buffer_word rowData   = 0x0; 
  SiDAQ::buffer_word cluRowData = 0x0;

  // loop over clusters on this sensor
  unsigned int nClu = 0;  // cluster counter
  unsigned int nAdc = 0;  // ADC word counter, needed for padding
  for ( ;  m_sortedClusters.end() != clusterIter && sensor == (*clusterIter)->channelID().sensor(); ++clusterIter) {

    const LHCb::VeloCluster* clu = *clusterIter;

    unsigned int numStrips = clu->size();
    bool highThresh = clu->highThreshold();
    double interStripFrac = clu->interStripFraction();
    unsigned int strip = clu->channelID().strip(); 

    // encode cluster position
    VeloClusterWord vcw(strip,interStripFrac,numStrips,highThresh);
    SiDAQ::buffer_word packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());
    
    //encode adc values
    const std::vector< std::pair<int, unsigned int> >& stripSignals = clu->stripValues();
    for (unsigned int i=0; i<stripSignals.size(); ++i) {
      double adcCount = stripSignals[i].second;

      // create new adc word
      bool endOfCluster = (stripSignals.size() == i+1);
      SiADCWord aw(adcCount,endOfCluster);

      rowData |= (aw.value() << 
          ((nAdc % VeloDAQ::adc_per_buffer) << VeloDAQ::adc_shift));

      ++nAdc;
      if (nAdc % VeloDAQ::adc_per_buffer == 0) {
        m_clusterADCBuffer.push_back ( rowData );
        //reset rowData
        rowData = 0x0;
      }

    }

    // store the cluster position
    cluRowData |= (packedCluster << ((nClu % VeloDAQ::clu_per_buffer) << VeloDAQ::clu_shift));
    ++nClu;
    if ( nClu % VeloDAQ::clu_per_buffer == 0 ) {
      m_clusterPosBuffer.push_back( cluRowData );
      // clear the cluster buffer 
      cluRowData = 0x0 ; 
    }
  }

  // if there were an odd number of clusters, sort the last cluster row
  if (  nClu % VeloDAQ::clu_per_buffer != 0 ) {
    m_clusterPosBuffer.push_back ( cluRowData  );
  }
  // check row data is not empty, store remaining adcs
  if ( nAdc % VeloDAQ::adc_per_buffer != 0 ) {
    m_clusterADCBuffer.push_back ( rowData );
  }

  // add bank header
  // the actual PCN is not available and there are no banks with errors in stripping
  SiHeaderWord hw(nClu,0,0); 
  SiDAQ::buffer_word bankHeader = hw.value();
  m_rawData.push_back ( bankHeader );

  // add clusters positions  
  std::vector<SiDAQ::buffer_word>::iterator tmpCp = m_clusterPosBuffer.begin();
  for ( ; tmpCp != m_clusterPosBuffer.end() ; ++tmpCp ) {      
    m_rawData.push_back ( *tmpCp ) ;
  }

  // add adc values 
  std::vector<SiDAQ::buffer_word>::iterator tmpAdc = m_clusterADCBuffer.begin();
  for ( ; tmpAdc != m_clusterADCBuffer.end() ; ++tmpAdc ) {
    m_rawData.push_back ( *tmpAdc ) ;
  }

  // Find number of padding bytes at the end of bank an compute
  // raw bank size in bytes, including the 4 byte header but
  // *without* the padding bytes at the end. 
  // The number of padding bytes is completely determined by
  // the number of ADC words in the raw bank.
  int adcRemainder = nAdc%VeloDAQ::adc_per_buffer;
  m_bankSizeInBytes = sizeof(SiDAQ::buffer_word)*m_rawData.size()  
    - (adcRemainder ? sizeof(SiDAQ::buffer_word)-adcRemainder*VeloDAQ::adc_word_size : 0);

  return nClu;
}

bool VeloClustersToRaw::selfTest() 
{
  info() << "==> selfTest" << endmsg;

  bool passed = false;

  const std::vector<LHCb::RawBank*>& banksOut = m_rawEventOut->banks(LHCb::RawBank::Velo);

  LHCb::VeloClusters* clusters = new LHCb::VeloClusters();
  std::vector<LHCb::RawBank*>::const_iterator outIter = banksOut.begin();
  for ( ; outIter != banksOut.end(); ++outIter ) {

    LHCb::RawBank* outBank = (*outIter);

    const DeVeloSensor* sensor = m_velo->sensor( outBank->sourceID() );
    int byteCount = 0;
    std::string errMsg;

    VeloDAQ::decodeRawBankToClustersV3(static_cast<SiDAQ::buffer_word*>(outBank->data()), 
                                       sensor, false, clusters, byteCount, errMsg, false);
  }
  
  std::vector<const LHCb::VeloCluster*> decodedClusters(clusters->size());
  std::copy(clusters->begin(),clusters->end(),decodedClusters.begin());
  std::sort( decodedClusters.begin(), decodedClusters.end(), 
      VeloDAQ::veloClusterPtrLessThan());

  std::vector<const LHCb::VeloCluster*>::const_iterator iOut = decodedClusters.begin();
  std::vector<const LHCb::VeloCluster*>::const_iterator iIn = m_sortedClusters.begin();

  info() << "Number of clusters: ref=" << m_sortedClusters.size() << " out=" << decodedClusters.size() << endmsg;
  if (  m_sortedClusters.size() != decodedClusters.size() ) {
    delete clusters;
    return passed;
  }

  passed = true;
  
  for ( ; iOut != decodedClusters.end(); ++iIn, ++iOut ) {
    const LHCb::VeloCluster* inClu = *iIn;
    const LHCb::VeloCluster* outClu = *iOut;
    
    if ( inClu->channelID() != outClu->channelID() ) {
      error() << "Channel IDs do not match: ref=(" << inClu->channelID().sensor() << "," << inClu->channelID().strip()
        << ") out=(" << outClu->channelID().sensor() << "," << outClu->channelID().strip() << ")" << endmsg;
      passed = false;
    }

    if ( inClu->size() != outClu->size() ) {
      error() << "Cluster sizes do not match: ref=" << inClu->size()
        << " out=" << outClu->size() << endmsg;
      passed = false;
    } else {
      for (unsigned int i=0; i<outClu->size(); ++i) {
        if ( inClu->strip(i) != outClu->strip(i) ) {
          error() << "Cluster strips do not match: ref=" << inClu->strip(i)
            << " out=" << outClu->strip(i) << endmsg;
          passed = false;
        } 

        if ( inClu->adcValue(i) != outClu->adcValue(i) ) {
          error() << "Strip ADC values do not match: ref=" << inClu->adcValue(i)
            << " out=" << outClu->adcValue(i) << endmsg;
          passed = false;
        } 

      }
    }

    if ( inClu->totalCharge() != outClu->totalCharge() ) {
      error() << "Cluster charges do not match: ref=" << inClu->totalCharge()
        << " out=" << outClu->totalCharge() << endmsg;
      passed = false;
    }

    if ( inClu->highThreshold() != outClu->highThreshold() ) {
      error() << "Threshold flags do not match: ref=" << inClu->highThreshold()
        << " out=" << outClu->highThreshold() << endmsg;
      passed = false;
    }

    if ( inClu->pseudoSize() != outClu->pseudoSize() ) {
      error() << "Cluster pseudo sizes do not match: ref=" << inClu->pseudoSize()
        << " out=" << outClu->pseudoSize() << endmsg;
      passed = false;
    }

    if ( inClu->interStripFraction() != outClu->interStripFraction() ) {
      error() << "Inter strip fractions do not match: ref=" << inClu->interStripFraction()
        << " out=" << outClu->interStripFraction() << endmsg;
      passed = false;
    }

  }

  delete clusters;
  return passed;
}

