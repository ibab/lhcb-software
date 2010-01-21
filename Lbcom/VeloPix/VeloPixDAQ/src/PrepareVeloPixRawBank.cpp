// $Id: PrepareVeloPixRawBank.cpp,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
// Include files:
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VeloPixChannelID.h"
// Event
#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteCluster.h"
#include "Event/RawEvent.h"
#include "Event/BankWriter.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Local
#include "VeloPixClusterWord.h"
#include "VeloPixPatternWord.h"
#include "VeloPixToTWord.h"
#include "PrepareVeloPixRawBank.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloPixRawBank
//
// 2009-12-08 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PrepareVeloPixRawBank);

//=============================================================================
// Constructor
//=============================================================================
PrepareVeloPixRawBank::PrepareVeloPixRawBank(const std::string& name,
                                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("ClusterLocation", m_clusterLocation = 
                  "VeloPix/Clusters");
  declareProperty("RawEventLocation", m_rawEventLocation =
                  LHCb::RawEventLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVeloPixRawBank::~PrepareVeloPixRawBank(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode PrepareVeloPixRawBank::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  // Get a list of sensor numbers to identify empty sensors
  m_veloPixelDet = getDet<DeVeloPix>(DeVeloPixLocation::Default);
  std::vector<DeVeloPixSensor*>::const_iterator sIter =
                                 m_veloPixelDet->sensorsBegin();
  std::vector<DeVeloPixSensor*>::const_iterator sEnd =
                                 m_veloPixelDet->sensorsEnd();
  for(; sIter != sEnd; ++sIter) {
    m_sensorNumbers.push_back((*sIter)->sensorNumber());
  }
  std::sort(m_sensorNumbers.begin(),m_sensorNumbers.end());

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode PrepareVeloPixRawBank::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Get input containers of clusters
  const VeloPixClusters* clusters = 0;
  if(!exist<VeloPixClusters>(m_clusterLocation)) {
    error() << " ==> There are no VeloPixClusters in TES! " << endmsg;
    return StatusCode::FAILURE;
  } else {
    clusters = get<VeloPixClusters>(m_clusterLocation);
  }
  RawEvent* rawEvent;
  // Check if RawEvent exits
  if(exist<RawEvent>(m_rawEventLocation)) {
    rawEvent = get<RawEvent>(m_rawEventLocation);
  } else {
    // Create RawEvent
    rawEvent = new LHCb::RawEvent();
    eventSvc()->registerObject(m_rawEventLocation,rawEvent);
  }
  m_sortedClusters.clear();
  m_sortedClusters.resize(clusters->size());
  std::copy(clusters->begin(),clusters->end(),m_sortedClusters.begin());
  // Sort by sensor number
  std::stable_sort(m_sortedClusters.begin(),m_sortedClusters.end(),
                   sortLessBySensor);
  // Loop over all clusters and write one bank per sensor
  std::vector<const VeloPixCluster*>::const_iterator firstOnSensor,
                                                     lastOnSensor;
  lastOnSensor = firstOnSensor = m_sortedClusters.begin();
  unsigned int currentSensorNumber;
  int sensorIndex = -1;
  while(firstOnSensor != m_sortedClusters.end()) {
    currentSensorNumber = (*firstOnSensor)->key().sensor();
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
         (*lastOnSensor)->key().sensor() == currentSensorNumber) {
      ++lastOnSensor;
    }
    // Make and store the bank
    storeBank(currentSensorNumber,firstOnSensor,lastOnSensor,rawEvent);
    firstOnSensor = lastOnSensor;
  }

  return StatusCode::SUCCESS;
};       


//=============================================================================
// Store RawBank
//=============================================================================
void PrepareVeloPixRawBank::storeBank(int sensor,
                      std::vector<const VeloPixCluster*>::const_iterator begin,
                      std::vector<const VeloPixCluster*>::const_iterator end,
                      RawEvent* rawEvent)
{
  // Create new raw buffer in raw data cache, old one is cleared
  makeBank(begin,end);
  m_bankVersion = 1;
  if(m_veloPixelDet->sensor(sensor)) {
    debug() << "Sensor = " << sensor << endmsg;
    LHCb::RawBank* newBank =
          rawEvent->createBank(static_cast<SiDAQ::buffer_word>(sensor),
                               LHCb::RawBank::VeloPix,
                               m_bankVersion,
                               m_bankSizeInBytes,
                               &(m_rawData[0]));
    // Add new bank and pass memory ownership to raw event
    rawEvent->adoptBank(newBank,true);
  }
}


//=============================================================================
// Make RawBank 
//=============================================================================
void PrepareVeloPixRawBank::makeBank(
                std::vector<const VeloPixCluster*>::const_iterator begin,
                std::vector<const VeloPixCluster*>::const_iterator end)
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
  std::vector<const VeloPixCluster*>::const_iterator iC = begin;
  for(; iC != end; ++iC) {
    const LHCb::VeloPixCluster* cluster = *iC;
    // Get vectors of active channelIDs and Tots
    std::vector< std::pair<LHCb::VeloPixChannelID,int> > totVec;
    totVec = cluster->pixelHitVec();
    std::vector<LHCb::VeloPixChannelID> activeChIDs; activeChIDs.clear();
    std::vector<int> ToTs; ToTs.clear();
    for(std::vector< std::pair<LHCb::VeloPixChannelID,int> >::iterator
        id = totVec.begin(); id != totVec.end(); id++) {
      std::pair<LHCb::VeloPixChannelID,int> pair = *id;
      activeChIDs.push_back(pair.first);
      ToTs.push_back(pair.second);
    }
    // Pack LiteCluster
    double maxFract = 7; // 3 bits
    unsigned int xFract =
             int(ceil(cluster->lCluster().interPixelFractionX() * maxFract));
    unsigned int yFract =
             int(ceil(cluster->lCluster().interPixelFractionY() * maxFract));
    VeloPixClusterWord vplcw(cluster->lCluster().channelID().pixel(),
                  cluster->lCluster().clustToT(),xFract,yFract,
                  cluster->lCluster().isLongPixel());
    SiDAQ::buffer_word packedCluster;
    packedCluster = static_cast<SiDAQ::buffer_word>(vplcw.value());
    m_clusterLiteBuffer.push_back(packedCluster);
    // Pack centralPixel + pattern
    long pattern = findPattern(cluster->key(),activeChIDs);
    VeloPixPatternWord vppcw(cluster->key().pixel(),pattern);
    SiDAQ::buffer_word packedPattern;
    packedPattern = static_cast<SiDAQ::buffer_word>(vppcw.value());
    m_clusterPattBuffer.push_back(packedPattern);
    // Pack ToTs
    const size_t tot_shift = 4;
    int totBufferSize = 8;
    for(std::vector<int>::iterator it = ToTs.begin(); it != ToTs.end(); it++) {
      VeloPixToTWord vptw(*it);
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
long PrepareVeloPixRawBank::findPattern(LHCb::VeloPixChannelID centrChanID,
                            std::vector<LHCb::VeloPixChannelID> activeChIDs)
{
  const DeVeloPixSensor* sensor = m_veloPixelDet->sensor(centrChanID);    
  std::vector<LHCb::VeloPixChannelID> neighbsVec; neighbsVec.clear();
  StatusCode channelsValid;
  channelsValid = sensor->channelToNeighbours(centrChanID,neighbsVec);
  if(!channelsValid) Warning("channelToNeighbours failure");
  neighbsVec.push_back(centrChanID);
  std::sort(neighbsVec.begin(),neighbsVec.end(),sortLessByChannel);
  std::vector<int> patternVec; patternVec.clear();
  for(std::vector<LHCb::VeloPixChannelID>::iterator
      inc = neighbsVec.begin(); inc != neighbsVec.end(); ++inc) {
    LHCb::VeloPixChannelID nChan = *inc;
    int pattFlg = 0;
    for(std::vector<LHCb::VeloPixChannelID>::iterator
        ipd = activeChIDs.begin(); ipd != activeChIDs.end(); ipd++) {
      LHCb::VeloPixChannelID aChan = *ipd;
      if(nChan == aChan) {
        pattFlg = 1;
      }
    }
    patternVec.push_back(pattFlg);
  }
  double pattern = 0;
  int num = -1;
  for(std::vector<int>::iterator ip = patternVec.begin();
      ip != patternVec.end(); ip++) {
    num++;
    if(*ip != 0) pattern = pattern + pow(2,double(num));
  }
  return long(pattern);
}


//============================================================================
StatusCode PrepareVeloPixRawBank::finalize() {

  return GaudiAlgorithm::finalize();

}
