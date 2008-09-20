// $Id: STEventMerge.cpp,v 1.1 2008-09-20 12:56:35 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STDetSwitch.h"
#include "Event/STCluster.h"
#include "STEventMerge.h"
#include "Kernel/STDataFunctor.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STEventMerge );

STEventMerge::STEventMerge( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator){
  
  declareProperty("spills", m_spillsVector);

  m_inputLocation = STClusterLocation::ITClusters;
  declareProperty("clusterLocation", m_clusterLocation = "/Event/Raw/IT/MergedSpills");

}

STEventMerge::~STEventMerge()
{
  // STEventMerge destructor
}

StatusCode STEventMerge::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);


  std::vector<std::string>::const_iterator iSpillName = m_spillsVector.begin();
  while( iSpillName != m_spillsVector.end()) {
    m_spillPath.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  } // spillnames
  
  return StatusCode::SUCCESS;
}


StatusCode STEventMerge::execute()
{
  
  STClusters* fCont = new STClusters();
  fCont->reserve(5000);  
  std::vector<std::string>::const_iterator iterS = m_spillPath.begin();
  for ( ;iterS != m_spillPath.end(); ++iterS){
    STClusters* clusterCont = get<STClusters>(*iterS);
    STClusters::iterator iterC = clusterCont->begin();
    for (; iterC != clusterCont->end() ; ++iterC){

      STCluster* testCluster = fCont->object((*iterC)->channelID()) ;
      if (testCluster == 0){ 
	// not in the container insert     
        fCont->insert((*iterC)->clone(),(*iterC)->channelID());
      }
      else {
        // if this is a better cluster take it...
	if ((*iterC)->totalCharge() > testCluster->totalCharge()){
	  fCont->remove(testCluster);
          fCont->insert((*iterC)->clone(),(*iterC)->channelID()); 
	}
      }
    }
  } // spills 
   
  // sort 
  std::sort(fCont->begin(),fCont->end(), STDataFunctor::Less_by_Channel<const STCluster*>());

  put(fCont, m_clusterLocation);

  return StatusCode::SUCCESS;
}




