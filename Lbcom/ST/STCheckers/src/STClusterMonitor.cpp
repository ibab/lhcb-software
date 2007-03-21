// $Id: STClusterMonitor.cpp,v 1.2 2007-03-21 14:24:44 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// DigiEvent
#include "Event/STCluster.h"

// STDet
#include "STDet/DeSTDetector.h"

// from LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/STDetSwitch.h"

// local
#include "STClusterMonitor.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterMonitor );

//--------------------------------------------------------------------
//
//  STClusterMonitor : Monitoring class for the STClusters
//
//--------------------------------------------------------------------

STClusterMonitor::STClusterMonitor( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("DetType", m_detType = "TT"); 
}

STClusterMonitor::~STClusterMonitor() { }

StatusCode STClusterMonitor::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
    
  // detector element     
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // Determine the location of the STClusters
  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
 
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName + m_detType);

  return StatusCode::SUCCESS;
}

StatusCode STClusterMonitor::execute()
{
  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_clusterLocation);
  
  // number of digits
  plot((double)clusterCont->size(),1,"Number of clusters", 0.,5000., 500);

  // histograms per cluster
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus) fillHistograms(*iterClus);

  return StatusCode::SUCCESS;
}

//=========================================================================
// Fill histograms for a given cluster
//=========================================================================
void STClusterMonitor::fillHistograms(const STCluster* aCluster)
{
  // cluster Size 
  plot((double)aCluster->size(),"Size of cluster",-0.5,6.5,7);

  // high threshold
  plot((double)aCluster->highThreshold(),"High threshold",-0.5,1.5,2);

  // histogram by station
  const int iStation = aCluster->channelID().station();
  plot((double)iStation,"Number of clusters per station",-0.5,4.5,5);
 
  // by layer
  const int iLayer = aCluster->channelID().layer();
  plot((double)(10*iStation+iLayer),"Number of clusters per layer",
       -0.5,40.5,41);

  if (fullDetail() == true) {
 
    plot( (double)aCluster->pseudoSize(),"Pseudo size of cluster",
          -0.5,10.5,11);
    plot( (double)aCluster->interStripFraction(),"Interstrip fraction",
          -0.125, 1.125, 5);

    // neighbour sum
    plot(aCluster->neighbourSum(),"Neighbour sum", -16.5, 26.5, 43);
    plot(aCluster->neighbourSum()/aCluster->totalCharge(),
         "Relative neighbour sum", -1.02, 1.02, 51);

    if (aCluster->size() < 3) {
      plot(aCluster->neighbourSum(),"Neighbour sum (1- and 2-strip clusters)", 
           -16.5, 26.5, 43);
      plot(aCluster->neighbourSum()/aCluster->totalCharge(),
           "Relative neighbour sum (1- and 2-strip clusters)", -1.02, 1.02, 51);
    }

    const DeSTSector* aSector = m_tracker->findSector(aCluster->channelID());
    if (aSector != 0) {
      plot(aCluster->totalCharge(), "Charge of "+aSector->type()+" ladders",
           0., 200., 200);
      plot(m_sigNoiseTool->signalToNoise(aCluster), 
           "S/N of "+aSector->type()+" ladders",0., 100., 100);
    }
  }

  // end
  return;
}
