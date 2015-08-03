// Include files
// From Detector Element
#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"

// From Event
#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

// local
#include "VeloClusterFilter.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloClusterFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterFilter::VeloClusterFilter( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_velo(NULL)
{
  declareProperty("InputClusterLocation", m_inputClusterLocation =  LHCb::VeloClusterLocation::Default);
  declareProperty("InputLiteClusterLocation", m_inputLiteClusterLocation =  LHCb::VeloLiteClusterLocation::Default);
  declareProperty("OutputClusterLocation",m_outputClusterLocation= "/Event/Raw/Velo/ClustersCopy");
  declareProperty("OutputLiteClusterLocation",m_outputLiteClusterLocation= "/Event/Raw/Velo/LiteClustersCopy");
  declareProperty("MinimumNumberOfRClusters",m_minNRClustersCut=0);
  declareProperty("MinimumNumberOfPhiClusters",m_minNPhiClustersCut=0);
  declareProperty("MinimumNumberOfClusters",m_minNClustersCut=0);
  declareProperty("MaximumNumberOfRClusters",m_maxNRClustersCut=100000);
  declareProperty("MaximumNumberOfPhiClusters",m_maxNPhiClustersCut=100000);
  declareProperty("MaximumNumberOfClusters",m_maxNClustersCut=100000);
  declareProperty("FilterOption",m_filterCriterion="All");
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterFilter::~VeloClusterFilter()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloClusterFilter::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloClusterFilter::execute()
{
  bool isDebug   = msgLevel( MSG::DEBUG   );
  if(isDebug){  debug() << "==> Execute" << endmsg; }

  int countClusters=0;
  int countRClusters=0;
  int countPhiClusters=0;
  int totalClusters=0;

  const LHCb::VeloClusters* clusters = getIfExists<LHCb::VeloClusters>(m_inputClusterLocation);
  const LHCb::VeloLiteCluster::FastContainer* liteClusters =
    getIfExists<LHCb::VeloLiteCluster::FastContainer>(m_inputLiteClusterLocation);
  if( (NULL==clusters) && (NULL==liteClusters) ) {
    return Warning( "No velo clusters or liteClusters on the TES !!!" );
  }

  if( NULL != clusters ){

    LHCb::VeloClusters* filteredClusters = new LHCb::VeloClusters();
    filteredClusters->reserve(clusters->size());
    
    for (LHCb::VeloClusters::const_iterator ci =  clusters->begin(); ci != clusters->end(); ++ci) {
      LHCb::VeloCluster* cluster = *ci;
      if (passesFilter(cluster->channelID())) {
          filteredClusters->insert(cluster);
          incrementCounters(cluster->channelID(),countClusters,countRClusters,countPhiClusters);
      }
      ++totalClusters;
    }
    put(filteredClusters,m_outputClusterLocation);
  }
  
  if( NULL != liteClusters ){
    
    LHCb::VeloLiteCluster::FastContainer* filteredLiteClusters = new LHCb::VeloLiteCluster::FastContainer();
    filteredLiteClusters->reserve(liteClusters->size());
    
    for (LHCb::VeloLiteCluster::FastContainer::const_iterator ci =  liteClusters->begin(); ci != liteClusters->end(); ++ci) {
      if (passesFilter(ci->channelID())){
        filteredLiteClusters->push_back(*ci);
        incrementCounters(ci->channelID(),countClusters,countRClusters,countPhiClusters);
      }
      ++totalClusters;
    }
    put(filteredLiteClusters,m_outputLiteClusterLocation);
  }

  if(isDebug) debug() << "Number of \'" << m_filterCriterion << "\' clusters surviving = "<< countClusters
                      <<"(R:"<<countRClusters<<" ,P:"<<countPhiClusters<<")"<< " from " << totalClusters << "." << endmsg;
  
  if(countRClusters   < m_minNRClustersCut  ) setFilterPassed(false);
  if(countPhiClusters < m_minNPhiClustersCut) setFilterPassed(false);
  if(countClusters    < m_minNClustersCut   ) setFilterPassed(false);
  if(countRClusters   > m_maxNRClustersCut  ) setFilterPassed(false);
  if(countPhiClusters > m_maxNPhiClustersCut) setFilterPassed(false);
  if(countClusters    > m_maxNClustersCut   ) setFilterPassed(false);

  return StatusCode::SUCCESS;
}

//=============================================================================
void VeloClusterFilter::incrementCounters(LHCb::VeloChannelID id,int& countClusters,int& countRClusters,int& countPhiClusters)
{
  unsigned int sensorNumber = id.sensor();
  if(sensorNumber<64){
    ++countRClusters;
  }else if(sensorNumber<128){
    ++countPhiClusters;
  }
  ++countClusters;
}

bool VeloClusterFilter::passesFilter(LHCb::VeloChannelID id)
{
  unsigned int sensorNumber = id.sensor();

  if(m_filterCriterion == "All") return true;

  if(m_filterCriterion == "Left"){
    if(sensorNumber%2==0) return true;
    return false;
  }
  if(m_filterCriterion == "Right"){
    if(sensorNumber%2==1) return true;
    return false;
  }
  if(m_filterCriterion == "R"){
    if(sensorNumber<64) return true;
    return false;
  }
  if(m_filterCriterion == "Phi"){
    if(sensorNumber>=64&&
       sensorNumber<128) return true;
    return false;
  }
  if(m_filterCriterion == "PU"){
    if(sensorNumber>=128) return true;
    return false;
  }
  if(m_filterCriterion == "Overlap"){
    bool response = false;
    unsigned int stripNumber = id.strip();
    if(id.isRType() || id.isPileUp()){
      if(stripNumber< 512 || stripNumber>=1536) response=true;
    }else if(id.isPhiType()){
      unsigned int range=25;
      if(stripNumber<range) response=true;
      if(stripNumber>=2048-range && stripNumber<2048) response=true;
      if(stripNumber>= 683-range && stripNumber<683+range) response=true;
    }
    return response;
  }
  warning() << "Don't understand \'"<<m_filterCriterion<<"\'. Expect: All,Left,Right,R,Phi,PU or Overlap" << endmsg;
  return false;
}
