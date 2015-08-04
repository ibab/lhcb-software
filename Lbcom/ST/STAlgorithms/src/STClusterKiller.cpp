
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "STClusterKiller.h"
#include "Kernel/ISTClusterSelector.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterKiller )

STClusterKiller::STClusterKiller( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_clusterSelector(0)
 {

  declareProperty("SelectorType", m_selectorType = "STSelectClustersByChannel");
  declareSTConfigProperty("InputLocation",m_inputLocation, STClusterLocation::TTClusters); 
  declareSTConfigProperty("SelectorName", m_selectorName, detType()+"Killer");
}

STClusterKiller::~STClusterKiller()
{
  // STClusterKiller destructor
}

StatusCode STClusterKiller::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_clusterSelector = tool<ISTClusterSelector>(m_selectorType, m_selectorName);

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
    if (select == true){;
      deadClusters.push_back((*iterC)->key());
      ++counter("Dead");
    }
      ++counter("Processed");
  } // iterC
}

StatusCode STClusterKiller::finalize() {

  const double dead = counter("Dead").flag();
  const double processed = counter("Processed").flag();

  double killed = 0.0; 
  if (!LHCb::Math::Equal_To<double>()(processed, 0.0)){ 
     killed = dead/processed; 
  }
  info() << "Fraction of clusters killed " << 100* killed << " %"  << endmsg;
    

  return ST::AlgBase::finalize();
}


