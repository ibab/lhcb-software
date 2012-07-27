// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from FTEvent
#include "Event/FTCluster.h"

// local
#include "FTClusterMonitor.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : FTClusterMonitor
//
// 2012-07-05 : Eric Cogneras
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTClusterMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FTClusterMonitor::FTClusterMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("ClusterLocation" , m_clusterLocation =  LHCb::FTClusterLocation::Default, "Path to Clusters");
}
//=============================================================================
// Destructor
//=============================================================================
FTClusterMonitor::~FTClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTClusterMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // retrieve FTCluster
  const FTClusters* clustersCont = get<FTClusters>(m_clusterLocation);
  debug() <<"clustersCont->size() : " << clustersCont->size()<< endmsg;


  // Loop over FTCluster
  for (FTClusters::const_iterator iterCluster = clustersCont->begin(); iterCluster!=clustersCont->end();++iterCluster){
    FTCluster* cluster = *iterCluster;

    // draw cluster channelID
    plot(cluster->channelID(), "ClusChannelID", "Cluster ChannelID; Cluster ChannelID" , 
         0. , 1000000. ,10000);
    // draw cluster fractional part
    plot(cluster->fraction(), "ClusFraction", 
         "Cluster Fractional part; Cluster fractional part" , 0 , 1);
    // draw cluster width
    plot(cluster->size(),"ClusSize", "Cluster Size; Cluster Size" , 0 , 100);
    // draw cluster total adc charge
    plot(cluster->charge(),"ClusCharge","Cluster Charge; Cluster Charge" , 0 , 100);
    plot(cluster->charge(),"ClusChargeZoom","Cluster Charge (Zoom); Cluster Charge" , 0. , 16., 16);

   plot2D(cluster->size(), cluster->charge(), "ClusChargevsSize",
           "Cluster Charge vs. Cluster Size; Cluster Charge [ADC counts]; Cluster size [Nb of Channels]" ,
           0. , 16. ,16, 0. , 100., 100);

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTClusterMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
