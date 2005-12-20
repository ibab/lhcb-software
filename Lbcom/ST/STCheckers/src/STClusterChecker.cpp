// $Id: STClusterChecker.cpp,v 1.2 2005-12-20 16:08:11 cattanem Exp $
//
// This File contains the implementation of the STClusterChecker class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Histogramming
#include "AIDA/IHistogram1D.h"

#include "STClusterChecker.h"

#include "Kernel/ISTSignalToNoiseTool.h"

// Event
#include "Event/STCluster.h"

// xml geometry
#include "STDet/DeSTDetector.h"

#include "Kernel/STDetSwitch.h"

#include "Kernel/STDataFunctor.h"


using namespace LHCb;

static const AlgFactory<STClusterChecker> s_Factory;
const IAlgFactory& STClusterCheckerFactory = s_Factory;

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

  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
 
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName,m_sigNoiseToolName + m_detType);
 

  return StatusCode::SUCCESS;
}

StatusCode STClusterChecker::execute(){

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

StatusCode STClusterChecker::fillHistograms(const STCluster* aCluster) {

  // fill histos per digit

  // number of deposits that contribute
  plot((double)aCluster->digits().size(),"num digit per cluster",-0.5,10.5,11);

  // histogram by station
  const int iStation = aCluster->channelID().station();
  plot((double)iStation,"n clus per stat",-0.5,11.5,11);
 
  // by layer
  const int iLayer = aCluster->channelID().layer();
  plot((double)(100*iStation+iLayer),"n clus per layer",-0.5,600.5,601);

  if (fullDetail() == true){

    DeSTSector* aSector = m_tracker->findSector(aCluster->channelID());
    if (aSector != 0){

      double totCharge = std::accumulate(aCluster->digits().begin(),aCluster->digits().end(),0.,
                           STDataFunctor::Accumulate_Charge<const LHCb::STDigit*>());
      plot(totCharge,aSector->type(), 0., 200., 200);
      plot(m_sigNoiseTool->signalToNoise(aCluster),aSector->type(), 0., 200., 200);
    }  

  }

  // end
  return StatusCode::SUCCESS;
}


















