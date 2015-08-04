// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "STLiteClusterKiller.h"
#include "Kernel/ISTChannelIDSelector.h"


// boost
#include <boost/foreach.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STLiteClusterKiller )

STLiteClusterKiller::STLiteClusterKiller( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_clusterSelector(0)
 {

  declareProperty("SelectorType", m_selectorType = "STRndmChannelIDSelector");
  declareSTConfigProperty("InputLocation",m_inputLocation, STLiteClusterLocation::TTClusters); 
  declareSTConfigProperty("SelectorName",m_selectorName , detType() + "LiteKiller");
}

STLiteClusterKiller::~STLiteClusterKiller()
{
  // STLiteClusterKiller destructor
}

StatusCode STLiteClusterKiller::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_clusterSelector = tool<ISTChannelIDSelector>(m_selectorType, m_selectorName);

  return StatusCode::SUCCESS;
}


StatusCode STLiteClusterKiller::execute()
{
  
  STLiteCluster::STLiteClusters* clusterCont = get<STLiteCluster::STLiteClusters>(m_inputLocation);

  // make list of clusters to keep
  std::vector<STLiteCluster> selected; selected.reserve(clusterCont->size());
  livingClusters(clusterCont,selected);

  // clear the input container
  clusterCont->clear();

  // add the selected clusters to input
  BOOST_FOREACH( STLiteCluster clus, selected){
    clusterCont->push_back(clus);  
  }  // for each 

  return StatusCode::SUCCESS;
}

void STLiteClusterKiller::livingClusters(const LHCb::STLiteCluster::STLiteClusters* clusterCont,
                                      std::vector<LHCb::STLiteCluster>& selectedClusters) const{


  // select the cluster to remove
  STLiteCluster::STLiteClusters::const_iterator iterC = clusterCont->begin();
  for ( ; iterC != clusterCont->end(); ++iterC){
    const bool select = (*m_clusterSelector)(iterC->channelID());
    if (select == true){
      // we kill it
    }
    else {
      selectedClusters.push_back(*iterC);
    }
  } // iterC
}


