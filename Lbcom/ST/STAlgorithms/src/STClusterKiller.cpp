// $Id: STClusterKiller.cpp,v 1.4 2009-02-10 09:44:15 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"


// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "STClusterKiller.h"
#include "Kernel/ISTClusterSelector.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterKiller );

STClusterKiller::STClusterKiller( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_clusterSelector(0)
 {

  declareProperty("Selector", m_selectorType = "STRndmClusterSelector");
  declareSTConfigProperty("InputLocation",m_inputLocation, STClusterLocation::TTClusters); 
}

STClusterKiller::~STClusterKiller()
{
  // STClusterKiller destructor
}

StatusCode STClusterKiller::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_clusterSelector = tool<ISTClusterSelector>(m_selectorType, "Selector", this);

  return StatusCode::SUCCESS;
}


StatusCode STClusterKiller::execute()
{
  
  STClusters* clusterCont = get<STClusters>(m_inputLocation);

  // make list of clusters to remove
  std::vector<STChannelID> chanList; chanList.reserve(100);
  removedClusters(clusterCont,chanList);


  // remove from the container
  std::vector<STChannelID>::reverse_iterator iterVec = chanList.rbegin(); 
  for (;  iterVec != chanList.rend(); ++iterVec){
    clusterCont->erase(*iterVec);
  } // iterVec

  return StatusCode::SUCCESS;
}

void STClusterKiller::removedClusters(const LHCb::STClusters* clusterCont,
                                      std::vector<LHCb::STChannelID>& deadClusters) const{


  // select the cluster to remove
  STClusters::const_iterator iterC = clusterCont->begin();
  for ( ; iterC != clusterCont->end(); ++iterC){
    const bool select = (*m_clusterSelector)(*iterC);
    if (!select){
      deadClusters.push_back((*iterC)->key());
    }
  } // iterC
}


