#include "GaudiKernel/AlgFactory.h"

#include "Event/VPCluster.h"

#include "PrPixelClustering.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrPixelClustering
//
// 2014-02-26 : Kurt Rinnert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PrPixelClustering)

//=============================================================================
// Constructor
//=============================================================================
PrPixelClustering::PrPixelClustering(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_isDebug(false)
{
  declareProperty("ClusterLocation", m_clusterLocation = LHCb::VPClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrPixelClustering::~PrPixelClustering(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode PrPixelClustering::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;

  m_hitManager = tool<PrPixelHitManager>("PrPixelHitManager");
  m_hitManager->setClusterLocation(m_clusterLocation);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode PrPixelClustering::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  m_hitManager->storeClusters();

  return StatusCode::SUCCESS;
}



