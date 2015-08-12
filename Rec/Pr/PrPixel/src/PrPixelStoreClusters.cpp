#include "Event/VPCluster.h"

#include "PrPixelStoreClusters.h"

DECLARE_ALGORITHM_FACTORY(PrPixelStoreClusters)

//=============================================================================
// Constructor
//=============================================================================
PrPixelStoreClusters::PrPixelStoreClusters(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator), m_isDebug(false) {
  declareProperty("ClusterLocation",
                  m_clusterLocation = LHCb::VPClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrPixelStoreClusters::~PrPixelStoreClusters() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode PrPixelStoreClusters::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if (m_isDebug) debug() << "==> Initialise" << endmsg;

  m_hitManager = tool<PrPixelHitManager>("PrPixelHitManager");
  m_hitManager->setClusterLocation(m_clusterLocation);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode PrPixelStoreClusters::execute() {
  if (m_isDebug) debug() << "==> Execute" << endmsg;

  m_hitManager->storeClusters();

  return StatusCode::SUCCESS;
}
