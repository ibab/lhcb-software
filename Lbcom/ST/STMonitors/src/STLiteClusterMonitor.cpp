// DigiEvent
#include "Event/STCluster.h"
#include "Event/STSummary.h"

// STDet
#include "STDet/DeSTDetector.h"


// local
#include "STLiteClusterMonitor.h"

using namespace LHCb;

namespace ST {
  DECLARE_ALGORITHM_FACTORY( STLiteClusterMonitor )
}

//--------------------------------------------------------------------
//
//  STLiteClusterMonitor : Monitoring class for the STClusters
//
//--------------------------------------------------------------------

ST::STLiteClusterMonitor::STLiteClusterMonitor( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
 ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer

  declareSTConfigProperty("InputData", m_clusterLocation , STLiteClusterLocation::TTClusters);

  setForcedInit(); // know we want that detector element
}

ST::STLiteClusterMonitor::~STLiteClusterMonitor() { }

StatusCode ST::STLiteClusterMonitor::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");

  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  return StatusCode::SUCCESS;
}

StatusCode ST::STLiteClusterMonitor::execute()
{
  // retrieve clusters
  const STLiteClusters* clusterCont = get<STLiteClusters>(m_clusterLocation);
  
  // number of digits
  plot((double)clusterCont->size(),"#clusters", "Number of clusters", 0.,5000., 500);

  // histograms per cluster
  STLiteClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus) fillHistograms(*iterClus);

  return StatusCode::SUCCESS;
}

//=========================================================================
// Fill histograms for a given cluster
//=========================================================================
void ST::STLiteClusterMonitor::fillHistograms(const STLiteCluster& aCluster)
{
  // cluster Size 
  plot((double)aCluster.pseudoSize(),"pseudoSize" ,"pseudoSize of cluster",-0.5,6.5,7);

  // high threshold
  plot((double)aCluster.highThreshold(),"highThres", "High threshold",-0.5,1.5,2);

  // histogram by station
  const int iStation = aCluster.station();
  plot((double)iStation,"clustersPerStation","Number of clusters per station",-0.5,4.5,5);
 
  // by layer
  const int iLayer = aCluster.layer();
  plot((double)(10*iStation+iLayer),"clustersPerLayer","Number of clusters per layer",
       -0.5,40.5,41);
   
  plot( (double)aCluster.interStripFraction(),"interstrip","Interstrip fraction",
          -0.125, 1.125, 5);

}
