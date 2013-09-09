// Include files:
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
// Event
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
#include "Event/RawEvent.h"
#include "Event/BankWriter.h"
// VPelDet
#include "VPDet/DeVP.h"
// Local
#include "VPClusterWord.h"
#include "VPPatternWord.h"
#include "VPToTWord.h"
#include "PrepareVPRawBank.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVPRawBank
//
// 2009-12-08 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PrepareVPRawBank)

//=============================================================================
// Constructor
//=============================================================================
PrepareVPRawBank::PrepareVPRawBank(const std::string& name,
                                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_bankVersion(1)
  , m_bankSizeInBytes(0)
  , m_isDebug(false)
  , m_isVerbose(false)
  , m_vPelDet(NULL)
{
  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::VPClusterLocation );
  declareProperty("RawEventLocation", m_rawEventLocation =
                  LHCb::RawEventLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVPRawBank::~PrepareVPRawBank(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode PrepareVPRawBank::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  // Get a list of sensor numbers to identify empty sensors
  m_vPelDet = getDet<DeVP>(DeVPLocation::Default);
  std::vector<DeVPSensor*>::const_iterator sIter =
                                 m_vPelDet->sensorsBegin();
  std::vector<DeVPSensor*>::const_iterator sEnd =
                                 m_vPelDet->sensorsEnd();
  for(; sIter != sEnd; ++sIter) {
    m_sensorNumbers.push_back((*sIter)->sensorNumber());
  }
  std::sort(m_sensorNumbers.begin(),m_sensorNumbers.end());

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode PrepareVPRawBank::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Get input containers of clusters
  const VPClusters* clusters = getIfExists<VPClusters>(m_clusterLocation);
  if( NULL == clusters ) {
    return Error( " ==> There are no VPClusters in TES! " );
  }

  // Check if RawEvent exits
  RawEvent* rawEvent = getIfExists<RawEvent>(m_rawEventLocation);
  if( NULL == rawEvent ) {
    // Create RawEvent
    rawEvent = new LHCb::RawEvent();
    put(rawEvent,m_rawEventLocation);
  }
  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(),clusters->end(),m_sortedClusters.begin());
  // Sort by sensor number
  std::stable_sort(m_sortedClusters.begin(),m_sortedClusters.end(),
                   sortLessBySensor);
  // Loop over all clusters and write one bank per sensor
  std::vector<const VPCluster*>::const_iterator firstOnSensor,
                                                     lastOnSensor;
  lastOnSensor = firstOnSensor = m_sortedClusters.begin();
  unsigned int currentSensorNumber;
  int sensorIndex = -1;
  while(firstOnSensor != m_sortedClusters.end()) {
    currentSensorNumber = (*firstOnSensor)->key().module();
    // Move to next on list of expected sensors
    sensorIndex++;
    // Check there was not a missing sensor
    while(currentSensorNumber != m_sensorNumbers[sensorIndex]) {
      // Store an empty bank
      storeBank(m_sensorNumbers[sensorIndex],lastOnSensor,lastOnSensor,
                rawEvent);
      if(m_isDebug) debug() << "Added empty bank for sensor "
                            << m_sensorNumbers[sensorIndex] << endreq;
      // Move to next on list, try again
      sensorIndex++;
    }
    while(lastOnSensor != m_sortedClusters.end() &&
         (*lastOnSensor)->key().module() == currentSensorNumber) {
      ++lastOnSensor;
    }
    // Make and store the bank
    storeBank(currentSensorNumber,firstOnSensor,lastOnSensor,rawEvent);
    firstOnSensor = lastOnSensor;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
/// Store RawBank
//=============================================================================
void PrepareVPRawBank::storeBank(int sensor,
                      std::vector<const VPCluster*>::const_iterator begin,
                      std::vector<const VPCluster*>::const_iterator end,
                      RawEvent* rawEvent) {

  // Create new raw buffer in raw data cache, old one is cleared.
  makeBank(begin, end);
  if (m_isDebug) debug() << "Sensor = " << sensor << endmsg;
  LHCb::RawBank* bank = rawEvent->createBank(static_cast<SiDAQ::buffer_word>(sensor),
                                             LHCb::RawBank::VP,
                                             m_bankVersion,
                                             m_bankSizeInBytes,
                                             &(m_rawData[0]));
  // Add new bank and pass memory ownership to raw event.
  rawEvent->adoptBank(bank, true);

}


//=============================================================================
// Make RawBank 
//=============================================================================
void PrepareVPRawBank::makeBank(
                std::vector<const VPCluster*>::const_iterator begin,
                std::vector<const VPCluster*>::const_iterator end)
{
  // Clear bank
  m_rawData.clear();
  // Work out number of clusters for this sensor uses 1st 32 bits
  int numClu = end - begin;
  if(m_isVerbose) {
    verbose() << "Number of clusters for sensor:" << numClu << endmsg;
  }
  // Header
  SiHeaderWord hw(numClu,0,0);
  SiDAQ::buffer_word pcnAndNumClu = hw.value();
  // For storing 32 bit 'rows' of lCusters, centralPixels + patterns and ToTs
  m_clusterLiteBuffer.clear();
  m_clusterPattBuffer.clear();
  m_clusterToTsBuffer.clear();
  // Loop over clusters in range defined by iterator
  int nToT = 0;
  SiDAQ::buffer_word totRowData = 0x0;
  std::vector<const VPCluster*>::const_iterator iC = begin;
  for(; iC != end; ++iC) {
    const LHCb::VPCluster* cluster = *iC;
    // Get vectors of active channelIDs and Tots
    std::vector< std::pair<LHCb::VPChannelID,int> > totVec;
    totVec = cluster->pixelHitVec();
    std::vector<LHCb::VPChannelID> activeChIDs; activeChIDs.clear();
    std::vector<int> ToTs; ToTs.clear();
    for(std::vector< std::pair<LHCb::VPChannelID,int> >::iterator
        id = totVec.begin(); id != totVec.end(); id++) {
      std::pair<LHCb::VPChannelID,int> pair = *id;
      activeChIDs.push_back(pair.first);
      ToTs.push_back(pair.second);
    }
    // Pack LiteCluster
    double maxFract = 7; // 3 bits
    unsigned int xFract =
             int(ceil(cluster->lCluster().interPixelFractionX() * maxFract));
    unsigned int yFract =
             int(ceil(cluster->lCluster().interPixelFractionY() * maxFract));
    VPClusterWord vplcw(cluster->lCluster().channelID().pixel(),
                  cluster->lCluster().clustToT(),xFract,yFract,
                  cluster->lCluster().isLongPixel());
    SiDAQ::buffer_word packedCluster;
    packedCluster = static_cast<SiDAQ::buffer_word>(vplcw.value());
    m_clusterLiteBuffer.push_back(packedCluster);
    // Pack centralPixel + pattern
    long pattern = findPattern(cluster->key(),activeChIDs);
    VPPatternWord vppcw(cluster->key().pixel(),pattern);
    SiDAQ::buffer_word packedPattern;
    packedPattern = static_cast<SiDAQ::buffer_word>(vppcw.value());
    m_clusterPattBuffer.push_back(packedPattern);
    // Pack ToTs
    const size_t tot_shift = 4;
    int totBufferSize = 8;
    for(std::vector<int>::iterator it = ToTs.begin(); it != ToTs.end(); it++) {
      VPToTWord vptw(*it);
      totRowData |= (vptw.value() << nToT * tot_shift);
      ++nToT;
    }
    if(nToT == totBufferSize) m_clusterToTsBuffer.push_back(totRowData);
    if(nToT > totBufferSize) {
      totRowData = 0x0;
      nToT = 0;
    }
  }
  // Fill RawData
  m_rawData.push_back(pcnAndNumClu); // Add header
  // Add LiteClusters
  std::vector<SiDAQ::buffer_word>::iterator 
                                   tmpLC = m_clusterLiteBuffer.begin();
  for(; tmpLC != m_clusterLiteBuffer.end(); ++tmpLC) {
    m_rawData.push_back(*tmpLC);
  }
  // Add centralPixels + patterns
  std::vector<SiDAQ::buffer_word>::iterator 
                                   tmpCP = m_clusterPattBuffer.begin();
  for(; tmpCP != m_clusterPattBuffer.end(); ++tmpCP) {
    m_rawData.push_back(*tmpCP);
  }
  // Add ToTs
  std::vector<SiDAQ::buffer_word>::iterator 
                                   tmpT = m_clusterToTsBuffer.begin();
  for(; tmpT != m_clusterToTsBuffer.end(); ++tmpT) {
    m_rawData.push_back(*tmpT);
  }
  m_bankSizeInBytes = sizeof(SiDAQ::buffer_word) * m_rawData.size();

}


//============================================================================
// Determine 9 bit pattern
//============================================================================
long PrepareVPRawBank::findPattern(LHCb::VPChannelID centrChanID,
                            std::vector<LHCb::VPChannelID> activeChIDs)
{
  const DeVPSensor* sensor = m_vPelDet->sensorOfChannel(centrChanID);    
  std::vector<LHCb::VPChannelID> neighbsVec; neighbsVec.clear();
  StatusCode channelsValid;
  channelsValid = sensor->channelToNeighbours(centrChanID,neighbsVec);
  if(!channelsValid) Warning("channelToNeighbours failure").ignore();
  neighbsVec.push_back(centrChanID);
  std::sort(neighbsVec.begin(),neighbsVec.end(),sortLessByChannel);
  std::vector<int> patternVec; patternVec.clear();
  for(std::vector<LHCb::VPChannelID>::iterator
      inc = neighbsVec.begin(); inc != neighbsVec.end(); ++inc) {
    LHCb::VPChannelID nChan = *inc;
    int pattFlg = 0;
    for(std::vector<LHCb::VPChannelID>::iterator
        ipd = activeChIDs.begin(); ipd != activeChIDs.end(); ipd++) {
      LHCb::VPChannelID aChan = *ipd;
      if(nChan == aChan) {
        pattFlg = 1;
      }
    }
    patternVec.push_back(pattFlg);
  }
  long pattern = 0;
  int num = -1;
  for(std::vector<int>::iterator ip = patternVec.begin();
      ip != patternVec.end(); ip++) {
    num++;
    if(*ip != 0) pattern = pattern | (1 << num);
  }
  return pattern;
}
