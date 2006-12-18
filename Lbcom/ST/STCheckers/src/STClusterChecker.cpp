// $Id: STClusterChecker.cpp,v 1.8 2006-12-18 11:04:09 cattanem Exp $
//
// This File contains the implementation of the STClusterChecker class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// from DigiEvent
#include "Kernel/ISTSignalToNoiseTool.h"

// Event
#include "Event/STCluster.h"

// STDet
#include "STDet/DeSTDetector.h"

// from LHCbKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/STDataFunctor.h"

// local
#include "STClusterChecker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterChecker );

//--------------------------------------------------------------------
//
//  STClusterChecker : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

STClusterChecker::STClusterChecker(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("detType", m_detType = "TT"); 
  declareProperty("clusterLocation", 
                  m_clusterLocation = STClusterLocation::TTClusters);
}

STClusterChecker::~STClusterChecker(){
  // destructer
}

StatusCode STClusterChecker::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  StatusCode sc = GaudiHistoAlg::initialize();
 if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
 
    
  // detector element     
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  STDetSwitch::flip(m_detType,m_clusterLocation);
 
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName + m_detType);
 

  return StatusCode::SUCCESS;
}

StatusCode STClusterChecker::execute()
{
  // execute once per event

  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_clusterLocation);
  
  // number of digits
  plot((double)clusterCont->size(),1,"Num clusters", 0.,5000., 500);

  // histos per digit
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){
    this->fillHistograms(*iterClus);
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode STClusterChecker::fillHistograms(const STCluster* aCluster)
{
  // fill histos per digit

  // cluster Size 
  plot((double)aCluster->size(),"num digit per cluster",-0.5,10.5,11);

  // high threshold
  plot((double)aCluster->highThreshold(),"high threshold",-0.5,1.5,2);

  // histogram by station
  const int iStation = aCluster->channelID().station();
  plot((double)iStation,"n clus per stat",-0.5,11.5,11);
 
  // by layer
  const int iLayer = aCluster->channelID().layer();
  plot((double)(100*iStation+iLayer),"n clus per layer",-0.5,600.5,601);

  if (fullDetail() == true){
 
    plot( (double)aCluster->pseudoSize(), "pseudo size", -0.5, 10.5, 11 );
    plot( (double)aCluster->interStripFraction(), "interstrip frac", -0.125, 
         1.125, 5);

    // neighbour sum
    plot(aCluster->neighbourSum(),"nSum", -16.5, 16.5, 33);
    plot(aCluster->neighbourSum()/aCluster->totalCharge(),"relSum", 
         -1.02, 1.02, 51);

    if (aCluster->size() < 3){
      plot(aCluster->neighbourSum(),"nSum2", -16.5, 16.5, 33);
      plot(aCluster->neighbourSum()/aCluster->totalCharge(),"relSum2", 
           -1.02, 1.02, 51);
    }

    const DeSTSector* aSector = m_tracker->findSector(aCluster->channelID());
    if (aSector != 0){
      plot(aCluster->totalCharge(),aSector->type()+"_charge", 0., 200., 200);
      plot( m_sigNoiseTool->signalToNoise(aCluster), aSector->type()+"_sn", 
            0., 200., 200);
    }
  }

  // end
  return StatusCode::SUCCESS;
}
