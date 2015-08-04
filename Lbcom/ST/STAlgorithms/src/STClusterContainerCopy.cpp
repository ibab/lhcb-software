
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "STClusterContainerCopy.h"
#include "Kernel/ISTClusterSelector.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterContainerCopy )

STClusterContainerCopy::STClusterContainerCopy( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_clusterSelector(0)
 {

  declareProperty("SelectorType", m_selectorType = "STSelectClustersByChannel");
  declareSTConfigProperty("InputLocation",m_inputLocation, STClusterLocation::TTClusters);
  declareSTConfigProperty("OutputLocation",m_outputLocation, STClusterLocation::TTClusters+"Selected");
  declareSTConfigProperty("SelectorName", m_selectorName, detType()+"Selector");
}

STClusterContainerCopy::~STClusterContainerCopy()
{
  // STClusterContainerCopy destructor
}

StatusCode STClusterContainerCopy::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_clusterSelector = tool<ISTClusterSelector>(m_selectorType, m_selectorName);

  return StatusCode::SUCCESS;
}


StatusCode STClusterContainerCopy::execute()
{
  
  LHCb::STClusters* clusterCont = get<STClusters>(m_inputLocation);
  LHCb::STClusters* outputCont = new STClusters();
  put( outputCont, m_outputLocation);

  STClusters::const_iterator iterC = clusterCont->begin();
  for ( ; iterC != clusterCont->end(); ++iterC){
    const bool select = (*m_clusterSelector)(*iterC);
    if (select){
      LHCb::STCluster* newClus = (*iterC)->clone(); 
      outputCont->insert(newClus,(*iterC)->channelID());
      ++counter("Selected");
    }
  }

  ++counter("Processed");

  return StatusCode::SUCCESS;
}

StatusCode STClusterContainerCopy::finalize() {

  const double selected = counter("Selected").flag();
  const double processed = counter("Processed").flag();

  double fracSel = 0.0; 
  if (!LHCb::Math::Equal_To<double>()(processed, 0.0)){ 
     fracSel = selected/processed; 
  }
  info() << "Fraction of clusters selected " << 100* fracSel << " %"  << endmsg;
    
  return ST::AlgBase::finalize();
}


