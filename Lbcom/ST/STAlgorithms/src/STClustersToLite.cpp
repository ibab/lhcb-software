// $Id: STClustersToLite.cpp,v 1.3 2008-12-04 10:18:56 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel includes
#include "Kernel/STChannelID.h"
//#include "Kernel/STDetSwitch.h"
#include "Event/STCluster.h"

#include "STClustersToLite.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClustersToLite );

STClustersToLite::STClustersToLite( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator){
 
  declareSTConfigProperty("inputLocation", m_inputLocation, STClusterLocation::TTClusters); 
  declareSTConfigProperty("outputLocation", m_outputLocation , STLiteClusterLocation::TTClusters);

}

STClustersToLite::~STClustersToLite()
{
  // STClustersToLite destructor
}

StatusCode STClustersToLite::execute()
{
  
  // get the input data
  const STClusters* clusterCont = get<STClusters>(m_inputLocation);

  // output
  STLiteCluster::STLiteClusters* fCont = new STLiteCluster::STLiteClusters();
  fCont->reserve(clusterCont->size()); 
  put(fCont, m_outputLocation);

  STClusters::const_iterator iterC = clusterCont->begin();
  for (; iterC != clusterCont->end() ; ++iterC){
    fCont->push_back((*iterC)->liteCluster()); 
  } // loop clusters
   
  return StatusCode::SUCCESS;
}


