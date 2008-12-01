// $Id: STClusterMonitor.cpp,v 1.13 2008-12-01 16:28:06 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// DigiEvent
#include "Event/STCluster.h"
#include "Event/STSummary.h"

// STDet
#include "STDet/DeSTDetector.h"

// from LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"

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
 ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareSTConfigProperty("InputData", m_clusterLocation , STClusterLocation::TTClusters);
  declareSTConfigProperty("summaryLocation", m_summaryLocation , STSummaryLocation::TTSummary);

  setForcedInit(); // know we want that detector element
}

STClusterMonitor::~STClusterMonitor() { }

StatusCode STClusterMonitor::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");

  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
    
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName + detType());

  return StatusCode::SUCCESS;
}

StatusCode STClusterMonitor::execute()
{
  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);
  
  // number of digits
  plot((double)clusterCont->size(),1,"Number of clusters", 0.,5000., 500);

  // histograms per cluster
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus) fillHistograms(*iterClus);

  // summary info....
  if (exist<STSummary>(m_summaryLocation) == true){
    const STSummary* summary = get<STSummary>(m_summaryLocation);
    plot(summary->pcn(),10,"pcn", -0.5, 200.5, 201 );
    if (summary->hasError()) ++counter("Errors");
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// Fill histograms for a given cluster
//=========================================================================
void STClusterMonitor::fillHistograms(const STCluster* aCluster)
{
  // cluster Size 
  plot((double)aCluster->size(),2,"Size of cluster",-0.5,6.5,7);

  // high threshold
  plot((double)aCluster->highThreshold(),3,"High threshold",-0.5,1.5,2);

  // histogram by station
  const int iStation = aCluster->station();
  plot((double)iStation,3,"Number of clusters per station",-0.5,4.5,5);
 
  // by layer
  const int iLayer = aCluster->layer();
  plot((double)(10*iStation+iLayer),4,"Number of clusters per layer",
       -0.5,40.5,41);

  // by strip, modulo a few things....
  const unsigned int strip = aCluster->strip();
  plot(strip%8,"strip modulo 8", -0.5, 8.5, 9);
  plot(strip%32,"strip modulo 32", -0.5, 32.5, 33);
  plot(strip%128,"strip modulo 128", -0.5, 128.5, 129);


  if (fullDetail() == true) {
 
    plot( (double)aCluster->pseudoSize(),5,"Pseudo size of cluster",
          -0.5,10.5,11);
    plot( (double)aCluster->interStripFraction(),6,"Interstrip fraction",
          -0.125, 1.125, 5);

    // neighbour sum
    plot(aCluster->neighbourSum(),7,"Neighbour sum", -16.5, 26.5, 43);
    plot(aCluster->neighbourSum()/aCluster->totalCharge(),
         "Relative neighbour sum", -1.02, 1.02, 51);

    if (aCluster->size() < 3) {
      plot(aCluster->neighbourSum(),8,"Neighbour sum (1- and 2-strip clusters)", 
           -16.5, 26.5, 43);
      plot(aCluster->neighbourSum()/aCluster->totalCharge(),
           9,"Relative neighbour sum (1- and 2-strip clusters)", -1.02, 1.02, 51);
    }

    // the source id
    plot(aCluster->sourceID(),"sourceID", -0.5, 50.5, 51);

    // the spill
    plot(aCluster->spill(), "spill", -10.5, 10.5, 21);

    // charge and S/N
    plot(aCluster->totalCharge(),detectorType(aCluster->channelID())+"/1", "Charge",-0.5, 500.5, 501);
    plot(m_sigNoiseTool->signalToNoise(aCluster),detectorType(aCluster->channelID())+"/2","S/N",0., 100., 100);
   
  }
}
