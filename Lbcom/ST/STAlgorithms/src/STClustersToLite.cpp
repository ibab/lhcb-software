// $Id: STClustersToLite.cpp,v 1.1 2008-10-11 10:42:23 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"


// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"
#include "Event/STCluster.h"

#include "STClustersToLite.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClustersToLite );

STClustersToLite::STClustersToLite( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator){
 
  declareProperty("inputLocation", m_inputLocation = STClusterLocation::ITClusters); 
  declareProperty("outputLocation", m_outputLocation  = STLiteClusterLocation::ITClusters);

}

STClustersToLite::~STClustersToLite()
{
  // STClustersToLite destructor
}

StatusCode STClustersToLite::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  STDetSwitch::flip(m_detType,m_inputLocation);   
  STDetSwitch::flip(m_detType,m_outputLocation);
 
  return StatusCode::SUCCESS;
}


StatusCode STClustersToLite::execute()
{
  
  // get the input data
  STClusters* clusterCont = get<STClusters>(m_inputLocation);

  // output
  STLiteCluster::STLiteClusters* fCont = new STLiteCluster::STLiteClusters();
  fCont->reserve(clusterCont->size()); 
  put(fCont, m_outputLocation);

  STClusters::iterator iterC = clusterCont->begin();
  for (; iterC != clusterCont->end() ; ++iterC){
    fCont->push_back((*iterC)->liteCluster()); 
  } // loop clusters
   
  return StatusCode::SUCCESS;
}


