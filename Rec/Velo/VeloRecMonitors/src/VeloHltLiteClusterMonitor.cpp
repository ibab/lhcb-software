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
  declareProperty( "HistogramByZone", m_histogramByZone = true );
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

  counter( "# events" ) =0;
  counter( "# VeloLiteClusters" ) = 0;
  declareInfo("nEvents",counter( "# events" ).flag(),"Number of Events");
  declareInfo("nLiteClusters",counter( "# VeloLiteClusters" ).flag(),"Number of VeloLiteClusters");

  m_nRSensorsOn = 0;
  std::vector<DeVeloRType*>::const_iterator iterV = m_veloDet->rSensorsBegin();
  for (; iterV != m_veloDet->rSensorsEnd(); ++iterV){
    if((*iterV)->isReadOut()) m_nRSensorsOn++;
  }
  h_nClusters.resize(2);
  h_nClustersByZone.resize(2);
  for (unsigned int lr=0;lr<h_nClusters.size();lr++){
    std::string side="ASide";
    if(lr) side="CSide";
    std::string name = "nClusters_"+side;
    std::string title= "N(Clusters)/event ("+side+")";
    h_nClusters[lr] = book1D(name,title,0.,512.*m_nRSensorsOn, 128 );
    if(m_histogramByZone){
      std::vector<IHistogram1D*> list;
      list.resize(4);
      for (unsigned int i=0;i<list.size();i++){
        name = "nClusters_"+side+"_Zone"+boost::lexical_cast<std::string>(i);
        title= "N(Clusters)/event for R-zone "+boost::lexical_cast<std::string>(i)+" ("+side+")";
        list[i]=book1D(name, title, 0., 128.*m_nRSensorsOn, 128 );
      }
      h_nClustersByZone[lr]=list;
    }
  }

  h_nClustersVsZ.resize(2);
  h_nClustersVsZByZone.resize(2);
  for (unsigned int lr=0;lr<h_nClustersVsZ.size();lr++){
    std::string side="ASide";
    if(lr) side="CSide";
    std::string name = "nClustersVsZ_"+side;
    std::string title= "N(Clusters)/event vs z ("+side+")";
    h_nClustersVsZ[lr] = bookProfile1D(name,title,-177.5,752.5,31);
    if(m_histogramByZone){
      std::vector<IProfile1D*> list;
      list.resize(4);
      for (unsigned int i=0;i<list.size();i++){
        name = "nClustersVsZ_"+side+"_Zone"+boost::lexical_cast<std::string>(i);
        title= "N(Clusters)/event vs. z for R-zone"+boost::lexical_cast<std::string>(i)+" ("+side+")";
        list[i]=bookProfile1D(name, title,-177.5,752.5,31);
      }
      h_nClustersVsZByZone[lr]=list;
    }
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

  std::vector< std::vector<int> > nClusters;
  nClusters.resize(42);
  for (int i=0;i<42;i++){
    nClusters[i].resize(4);
  }

  LHCb::VeloLiteCluster::FastContainer::const_iterator ci =  m_liteClusters->begin();
  for(; ci != m_liteClusters->end(); ++ci ) {
    if(ci->isRType()){
      LHCb::VeloChannelID id = ci->channelID();
      unsigned int sensorNumber = id.sensor();
      const DeVeloRType* sensor = (DeVeloRType*)m_veloDet->sensor( sensorNumber );
      int globalZone = sensor->globalZoneOfStrip(id.strip());
      nClusters[sensorNumber][globalZone]++;
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
        sumAll+=nClusters[sensorNumber][globalZone];
        sum4Each[globalZone]+=nClusters[sensorNumber][globalZone];
        if(m_histogramByZone){
          h_nClustersVsZByZone[isRight][globalZone]->fill(sensor->z(),nClusters[sensorNumber][globalZone]);
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
    h_nClusters[isRight]->fill(sum);
  }
}
