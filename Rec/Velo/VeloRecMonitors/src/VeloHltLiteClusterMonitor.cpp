// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloHltLiteClusterMonitor.h"

// from Boost
#include <boost/lexical_cast.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloHltLiteClusterMonitor
//
// VELO clusters monitoring algorithm.
// Produces a reduced set of histograms from the liteCluster bank. 
// Proposed for use in the HLT
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloHltLiteClusterMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloHltLiteClusterMonitor::VeloHltLiteClusterMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
{
  declareProperty( "VeloLiteClusterLocation", m_liteClusterLocation = LHCb::VeloLiteClusterLocation::Default );
  declareProperty( "HistogramByZone", m_histogramByZone = false );
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloHltLiteClusterMonitor::~VeloHltLiteClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloHltLiteClusterMonitor::initialize() {

  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  setHistoTopDir( "" );

  counter( "# events" ) =0;
  counter( "# VeloLiteClusters" ) = 0;
  declareInfo("nEvents",counter( "# events" ).flag(),"Number of Events");
  declareInfo("nLiteClusters",counter( "# VeloLiteClusters" ).flag(),"Number of VeloLiteClusters");

  m_nRSensorsOn = 0;
  std::vector<DeVeloRType*>::const_iterator iterV = m_veloDet->rSensorsBegin();
  for (; iterV != m_veloDet->rSensorsEnd(); ++iterV){
    if((*iterV)->isReadOut()) m_nRSensorsOn++;
  }
  h_nClustersLargeRange.resize(2);
  h_nClustersSmallRange.resize(2);
  h_nClustersByZone.resize(2);
  for (unsigned int lr=0;lr<2;lr++){
    std::string side="ASide";
    if(lr) side="CSide";
    std::string name = "nClusters_"+side+"_LargeRange";
    std::string title= "N(R-clusters)/event ("+side+")";
    h_nClustersLargeRange[lr] = book1D(name,title,0.,512.*m_nRSensorsOn, 128 );
    name = "nClusters_"+side+"_SmallRange";
    h_nClustersSmallRange[lr] = book1D(name,title,0.,512, 128 );
    if(m_histogramByZone){
      std::vector<IHistogram1D*> list;
      list.resize(4);
      for (unsigned int i=0;i<list.size();i++){
        name = "nClusters_"+side+"_Zone"+boost::lexical_cast<std::string>(i);
        title= "N(R-clusters)/event for R-zone "+boost::lexical_cast<std::string>(i)+" ("+side+")";
        list[i]=book1D(name, title, 0., 512, 128 );
      }
      h_nClustersByZone[lr]=list;
    }
  }

  h_nClustersVsZ.resize(2);
  h_nClustersVsBunchId.resize(2);
  h_nClustersVsZByZone.resize(2);
  for (unsigned int lr=0;lr<h_nClustersVsZ.size();lr++){
    std::string side="ASide";
    if(lr) side="CSide";
    std::string name = "nClustersVsZ_"+side;
    std::string title= "N(R-clusters)/event vs z ("+side+")";
    h_nClustersVsZ[lr] = bookProfile1D(name,title,-177.5,752.5,31);
    name = "nClustersVsBunchId_"+side;
    title= "N(R-clusters)/event vs LHC bunch-ID ("+side+")";
    h_nClustersVsBunchId[lr] = bookProfile1D(name,title,-0.5,3563.5,3564);
    if(m_histogramByZone){
      std::vector<IProfile1D*> list;
      list.resize(4);
      for (unsigned int i=0;i<list.size();i++){
        name = "nClustersVsZ_"+side+"_Zone"+boost::lexical_cast<std::string>(i);
        title= "N(R-clusters)/event vs. z for R-zone"+boost::lexical_cast<std::string>(i)+" ("+side+")";
        list[i]=bookProfile1D(name, title,-177.5,752.5,31);
      }
      h_nClustersVsZByZone[lr]=list;
    }
  }

  m_nRClusters.resize(42);
  for (int i=0;i<42;i++){
    m_nRClusters[i].resize(4);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloHltLiteClusterMonitor::execute() {

  // Retrieve the VeloClusters
  // -------------------------
  StatusCode sc = veloLiteClusters();
  if( sc.isFailure() ) return sc;

  // Count the events monitored
  // --------------------------
  counter( "# events" ) += 1;

  // Perform monitoring
  // ------------------
  monitorLiteClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloHltLiteClusterMonitor::finalize() {

  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
StatusCode Velo::VeloHltLiteClusterMonitor::veloLiteClusters() {
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_liteClusterLocation << endmsg;

  if (!exist<LHCb::ODIN> (LHCb::ODINLocation::Default))
    return Error("The ODIN bank is not found",StatusCode::FAILURE,50);
  m_odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  if ( !exist<LHCb::VeloLiteCluster::FastContainer>(m_liteClusterLocation) ){
    return Warning( "No VeloClusters container found for this event !",StatusCode::FAILURE, 100 );
  }
  else {
    m_liteClusters = get<LHCb::VeloLiteCluster::FastContainer>(m_liteClusterLocation);
    if ( m_debugLevel ) debug() << "  -> N(liteClusters) on TES: " << m_liteClusters->size() <<endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloHltLiteClusterMonitor::monitorLiteClusters()
{
  unsigned int nclus = m_liteClusters->size();
  counter( "# VeloLiteClusters" ) += nclus;

  for (int i=0;i<42;i++){
    for (int j=0;j<4;j++){
      m_nRClusters[i][j]=0;
    }
  }    

  if(m_liteClusters->size()<4) return;

  LHCb::VeloLiteCluster::FastContainer::const_iterator ci =  m_liteClusters->begin();
  for(; ci != m_liteClusters->end(); ++ci ) {
    if(ci->isRType()){
      LHCb::VeloChannelID id = ci->channelID();
      unsigned int sensorNumber = id.sensor();
      const DeVeloRType* sensor = (DeVeloRType*)m_veloDet->sensor( sensorNumber );
      int globalZone = sensor->globalZoneOfStrip(id.strip());
      m_nRClusters[sensorNumber][globalZone]++;
    }
  }

  for (unsigned int isRight=0;isRight<2;isRight++){
    int sum4Each[4]={0,0,0,0};
    std::vector<DeVeloRType*>::const_iterator iter = m_veloDet->rSensorsBegin();
    for (iter = m_veloDet->rSensorsBegin(); iter != m_veloDet->rSensorsEnd(); ++iter){
      DeVeloRType* sensor = (*iter);
      if(isRight != sensor->isRight()) continue;
      unsigned int sensorNumber = sensor->sensorNumber();
      int sumAll=0;
      for( int globalZone=0 ; globalZone<4 ; ++globalZone ){
        sumAll+=m_nRClusters[sensorNumber][globalZone];
        sum4Each[globalZone]+=m_nRClusters[sensorNumber][globalZone];
        if(m_histogramByZone){
          h_nClustersVsZByZone[isRight][globalZone]->fill(sensor->z(),m_nRClusters[sensorNumber][globalZone]);
        }
      }
      h_nClustersVsZ[isRight]->fill(sensor->z(),sumAll);
    }
    int sum=0;
    for (int i=0;i<4;i++){
      sum+=sum4Each[i];
      if(m_histogramByZone){
        h_nClustersByZone[isRight][i]->fill(sum4Each[i]);
      }
    }
    h_nClustersSmallRange[isRight]->fill(sum);
    h_nClustersLargeRange[isRight]->fill(sum);

    h_nClustersVsBunchId[isRight]->fill(m_odin->bunchId(),sum);
  }
}
