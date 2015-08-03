#include "Event/RawEvent.h"
#include "VeloEvent/InternalVeloCluster.h"

#include "VeloDet/DeVelo.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "SiDAQ/SiRawBufferWord.h"

#include "VeloClusterWord.h"
#include "VeloRawBankVersions.h"
#include "VeloRawWordSizes.h"
#include "VeloClusterPtrLessThan.h"
#include "PrepareVeloRawBuffer.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloRawBuffer
//
// 2003-04-11 : Olivier Callot
// 2004-04-13 : modified and moved to Velo area Chris Parkes
// 2004-11-03 : modified to GaudiAlg dependance
// 2006-02-15 : David Jones, Kurt Rinnert: complete rewrite for 1MHz raw buffer
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PrepareVeloRawBuffer )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloRawBuffer::PrepareVeloRawBuffer( const std::string& name,
    ISvcLocator* pSvcLocator) 
: 
  GaudiAlgorithm (name , pSvcLocator),
  m_clusterLoc(LHCb::InternalVeloClusterLocation::Default),
  m_rawEventLoc(LHCb::RawEventLocation::Default),
  m_bankVersion(VeloDAQ::v3),
  m_bankSizeInBytes(0),
  m_velo(NULL)
{
  declareProperty("InternalVeloClusterLocation",m_clusterLoc=LHCb::InternalVeloClusterLocation::Default);
  declareProperty("RawEventLocation",m_rawEventLoc=LHCb::RawEventLocation::Default);
  declareProperty("DumpInputClusters",m_dumpInputClusters=false);
  declareProperty("BankVersion", m_bankVersion=VeloDAQ::v3);
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVeloRawBuffer::~PrepareVeloRawBuffer() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareVeloRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endmsg;

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

  // get a list of sensor numbers to identify empty sensors
  std::vector< DeVeloSensor* >::const_iterator  sIter = m_velo->sensorsBegin();
  std::vector< DeVeloSensor* >::const_iterator  sEnd = m_velo->sensorsEnd();
  for( ; sIter != sEnd ; ++sIter ){
    m_sensorNumbers.push_back((*sIter)->sensorNumber());
  }
  std::sort( m_sensorNumbers.begin() , m_sensorNumbers.end() );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloRawBuffer::execute() {

  bool isDebug   = msgLevel( MSG::DEBUG );


  if (isDebug) debug() << "==> Execute" << endmsg;

  // Get the input container
  // Get the InternalVeloClusters from their default location 
  const LHCb::InternalVeloClusters* clusters = getIfExists<LHCb::InternalVeloClusters>(m_clusterLoc);
  if( NULL == clusters ){
    return Error( " ==> There are no InternalVeloClusters in TES! " );
  }

  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(),clusters->end(),m_sortedClusters.begin());

  // Then sort the clusters by sensor number and local coordinate
  std::sort( m_sortedClusters.begin(), m_sortedClusters.end(), 
      VeloDAQ::veloClusterPtrLessThan());

  // dump input clusters to console, if requested
  if (m_dumpInputClusters) dumpInputClusters();


  // see whether the raw event exits
  LHCb::RawEvent* rawEvent = getIfExists<LHCb::RawEvent>(m_rawEventLoc);
  if( NULL == rawEvent ) {
    // raw rawEvent doesn't exist. We need to create it 
    rawEvent = new LHCb::RawEvent();
    put(rawEvent, m_rawEventLoc);
  } 


  // loop over all clusters and write one bank per sensor
  std::vector<const LHCb::InternalVeloCluster*>::const_iterator firstOnSensor,
    lastOnSensor;
  lastOnSensor = firstOnSensor =  m_sortedClusters.begin();

  int currentSensorNumber;  

  int sensorIndex = -1; // index of current sensor in list

  while (firstOnSensor != m_sortedClusters.end()) {

    currentSensorNumber = (*firstOnSensor)->sensor();

    sensorIndex++; // move to next on list of expected sensors
    // check there was not a missing sensor
    while( currentSensorNumber != m_sensorNumbers[sensorIndex] ){
      // store an empty bank
      storeBank(m_sensorNumbers[sensorIndex],lastOnSensor, lastOnSensor,
          rawEvent); 
      if (isDebug) debug() << "Added empty bank for sensor " 
        << m_sensorNumbers[sensorIndex] << endmsg;
      sensorIndex++; // move to next on list, try again
    }

    while (lastOnSensor != m_sortedClusters.end() && 
        (*lastOnSensor)->sensor() == currentSensorNumber) {
      ++lastOnSensor;
    }

    // make and store the bank
    storeBank(currentSensorNumber, firstOnSensor, lastOnSensor, rawEvent);

    firstOnSensor = lastOnSensor;
  }
  // add any empty banks from final sessor in the list
  sensorIndex++;// move to next on list of expected sensors
  while(sensorIndex < static_cast<int>(m_sensorNumbers.size())){
    // store an empty bank
    storeBank(m_sensorNumbers[sensorIndex],lastOnSensor, lastOnSensor,
        rawEvent); 
    if (isDebug) debug() << "Added empty bank for sensor " 
      << m_sensorNumbers[sensorIndex] << endmsg;
    sensorIndex++; // move to next on list, try again
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

void PrepareVeloRawBuffer::storeBank(
    int sensor,
    std::vector<const LHCb::InternalVeloCluster*>::const_iterator begin, 
    std::vector<const LHCb::InternalVeloCluster*>::const_iterator end,
    LHCb::RawEvent* rawEvent) {

  // create new raw buffer in raw data cache, old one is cleared
  makeBank(begin, end);

  // Sensor and TELL1 Id might be different, e.g in a test beam
  // setup.  Also it makes no sense to add a bank for a sensor that
  // has not Tell1 Id.  While this should never be attempted in the 
  // first place (if the geometry XML and CondDB is correct), it is
  // still a good idea to protect against this scenario. 
  unsigned int sourceId;
  if (m_velo->tell1IdBySensorNumber(sensor, sourceId)) {

    debug() << "Sensor = " << sensor << "Source ID = " << sourceId << endmsg;

    LHCb::RawBank* newBank = rawEvent->
      createBank(static_cast<SiDAQ::buffer_word>(sourceId),
          LHCb::RawBank::Velo,
          m_bankVersion,
          m_bankSizeInBytes, 
          &(m_rawData[0]));

    // add new bank and pass memory ownership to raw event
    rawEvent->adoptBank(newBank,true);
  } else {
    // we should not end up here if the geometry XMl and the
    // CondDB sensor/Tell1 mapping is consistent
    warning() 
      << "Plot to create bank for sensor " << sensor 
      << " with no TELL1 Id foiled." << endmsg; 
  }
}

unsigned int PrepareVeloRawBuffer::makeBank (
    std::vector<const LHCb::InternalVeloCluster*>::const_iterator begin, 
    std::vector<const LHCb::InternalVeloCluster*>::const_iterator end) 
{
  bool isVerbose = msgLevel( MSG::VERBOSE );

  // clear bank
  m_rawData.clear();

  // work out number of clusters for this sensor uses 1st 16 bits
  int numClu = end - begin;

  if(isVerbose) {
    verbose() <<"Number of clusters:" << numClu  <<endmsg;
  }  

  // pcn is next 8 bits: set to zero here (never any errors in sim)
  SiHeaderWord hw(numClu,0,0); 
  SiDAQ::buffer_word pcnAndNumClu = hw.value();

  // clear temporary buffers
  m_clusterPosBuffer.clear();
  m_clusterADCBuffer.clear();

  // for storing 32 bit 'rows' of adc values and cluster positions
  SiDAQ::buffer_word rowData   = 0x0; 
  SiDAQ::buffer_word cluRowData = 0x0;

  // loop over clusters in range defined by iterator 
  unsigned int nClu = 0;  // cluster counter
  unsigned int nAdc = 0;  // ADC word counter, used to to determine
  // number of padding bytes at end of raw bank 

  std::vector<const LHCb::InternalVeloCluster*>::const_iterator iC = begin;
  for ( ; iC != end ; ++iC) {

    const LHCb::InternalVeloCluster* clu = *iC;

    unsigned int numStrips = clu->size();
    bool highThresh = clu->hasHighThreshold();

    if(isVerbose) {
      verbose() <<"Reading cluster from sensor "
        << clu->sensor() << " with " <<
        numStrips << " strips." <<endmsg;
    }

    SiDAQ::buffer_word packedCluster;


    if( numStrips == 1 ) {  // single strip cluster

      double adcCount = clu->adcValue(0);

      if (adcCount > 127) {
        adcCount = 127;
      }
      VeloClusterWord vcw(clu->strip(0),0.0,1, highThresh);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());
      if ( isVerbose ) {
        verbose() <<"STRIP: " << clu->strip(0)
          << ",ADC: " << clu->adcValue( 0) << endmsg;
      }

      SiADCWord aw(adcCount,true);
      rowData |= (aw.value() << ((nAdc % VeloDAQ::adc_per_buffer) 
            << VeloDAQ::adc_shift));

      ++nAdc;
      if (nAdc % VeloDAQ::adc_per_buffer == 0) {
        m_clusterADCBuffer.push_back ( rowData );
        //reset rowData
        rowData = 0x0;
      }

      if ( isVerbose ) {
        verbose() <<"strip " << clu->strip(0)
                  << ",adc= " << aw.value() << endmsg;
      }
    } else {
      // multiple strip cluster
      // calculate weighted average position of cluster 
      const std::vector< std::pair<long,double> >& stripSignals = 
        clu->stripSignals();
      double sumADC = 0.;
      double sumADCWeightedeStrip = 0.;

      // loop over all strip signals 

      unsigned int i = 0;
      for (; i<stripSignals.size(); ++i) {
        unsigned int stripNumber = stripSignals[i].first;
        double adcCount = stripSignals[i].second;
        if (adcCount > 127) {
          adcCount = 127;
        }

        if(isVerbose) {
          verbose() << "ADC COUNT:" <<  adcCount << " STRIP:" << stripNumber
            << endmsg; 
        }

        // sum adc values and adc weighted strip numbers
        sumADC += adcCount;
        sumADCWeightedeStrip += static_cast<double>(i) * adcCount;
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

      double       cPos          = 0.0;
      double       interStripPos = 0.0;
      unsigned int channelPos    = 0;

      unsigned int stripNumber = stripSignals[0].first;
      switch (m_bankVersion) {
        case VeloDAQ::v2:
          cPos = (sumADCWeightedeStrip/sumADC) ; // get weighted mean
          channelPos = stripNumber+static_cast<unsigned int>(cPos); // without fractional par
          interStripPos = cPos - static_cast<unsigned int>(cPos); // fractional part
          break;
        case VeloDAQ::v3:
          // compute the weighted mean like it is done on the TELL1
          cPos = sumADCWeightedeStrip*static_cast<int>(65536./sumADC+0.5)/65536.;
          channelPos = stripNumber+static_cast<int>(cPos+1/16.0); // without fractional part
          interStripPos = cPos - static_cast<int>(cPos) + 1/16.0; // fractional part, will be rounded to nearest 1/8
          break;
      }

      VeloClusterWord vcw(channelPos, interStripPos, numStrips, highThresh);
      packedCluster = static_cast<SiDAQ::buffer_word>(vcw.value());

      if(isVerbose) {
        verbose() << "CPOS: " << cPos
          << " CHPOS: " << channelPos
          << " ISPOS: " << interStripPos
          << " WORD: " << format( "%8X" , packedCluster )
          <<endmsg;
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

  // write to buffer or return ? 

  if ( isVerbose ) {
    verbose()<< "Raw  output of cluster start" <<endmsg;
    verbose()<< "PCN error flag and number of clusters[" << format( "%10X" , pcnAndNumClu ) 
      << "]"<<endmsg;
  }

  // add PCN and R
  m_rawData.push_back ( pcnAndNumClu  );

  // add clusters positions  
  std::vector<SiDAQ::buffer_word>::iterator tmpCp = m_clusterPosBuffer.begin();
  for ( ; tmpCp != m_clusterPosBuffer.end() ; ++tmpCp ) {      
    if ( isVerbose  ) {

      verbose()<< "Cluster position[" << format( "%10X" , *tmpCp ) << "]"<<endmsg;
    }

    m_rawData.push_back ( *tmpCp ) ;
  }

  // add adc values 
  std::vector<SiDAQ::buffer_word>::iterator tmpAdc = m_clusterADCBuffer.begin();
  for ( ; tmpAdc != m_clusterADCBuffer.end() ; ++tmpAdc ) {
    if ( isVerbose ) {

      verbose()<< "Adc row[" << format( "%10X" , *tmpAdc ) << "]"<<endmsg;
    }

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

  if ( isVerbose) {
    verbose()<< "Raw  output of cluster end" <<endmsg;
  }

  return nClu;
}

void PrepareVeloRawBuffer::dumpInputClusters() const
{
  for (std::vector<const LHCb::InternalVeloCluster*>::const_iterator ci =  m_sortedClusters.begin(); 
      ci != m_sortedClusters.end(); 
      ++ci) {

    const LHCb::InternalVeloCluster* clu = *ci;

    unsigned int sensorNumber = clu->sensor();

    unsigned int centroidStrip = clu->strip(0);
    double channelPos = centroidStrip;

    // ensure 7 bit cut off for adc values
    std::vector<double> adcValues;
    for (int adci=0; adci<clu->size(); ++adci) {
      adcValues.push_back((clu->adcValue(adci) > 127 ? 127 : clu->adcValue(adci)));
    }

    if (clu->size() > 1) {
      double sumAdc = 0.0;
      double offset = 0;
      for (int strip = 0; strip < clu->size(); ++strip) {
        offset += strip*adcValues[strip];
        sumAdc += adcValues[strip];
      }
      channelPos += offset/sumAdc;
      centroidStrip = static_cast<unsigned int>(channelPos);
    }

    // interstrip position in 1/8 of strip pitch (as it is encoded in raw bank)
    double interStripPos = static_cast<unsigned int>((channelPos-centroidStrip)*8.0)/8.0;

    std::cout << "ENC::POSDUMP:"
      << " SN=" << sensorNumber
      << " CS=" << centroidStrip
      << " ISP=" << interStripPos
      << std::endl;

    std::cout << "ENC::ADCDUMP:"
      << " SN=" << sensorNumber;

    for (int adci=0; adci<clu->size(); ++adci) {
      std::cout << " " << adcValues[adci]
        << "@" << clu->strip(adci);
    }
    std::cout << std::endl;

  }
  return;
}
