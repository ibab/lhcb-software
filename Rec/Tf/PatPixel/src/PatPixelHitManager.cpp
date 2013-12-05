#include "TMath.h"
// Gaudi
#include "GaudiKernel/ToolFactory.h"

// Local
#include "PatPixelHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelHitManager
//
// 2012-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(PatPixelHitManager)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelHitManager::PatPixelHitManager(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent) :
    GaudiTool(type, name, parent), m_useSlopeCorrection(true) {

  declareInterface<PatPixelHitManager>(this);

}

//=============================================================================
// Destructor
//=============================================================================
PatPixelHitManager::~PatPixelHitManager() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPixelHitManager::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  // Get detector element.
  m_vp = getDet<DeVP>(DeVPLocation::Default);
  // Make sure we are up-to-date on populated VELO stations
  registerCondition((*(m_vp->leftSensorsBegin()))->geometry(), &PatPixelHitManager::rebuildGeometry);
  registerCondition((*(m_vp->rightSensorsBegin()))->geometry(), &PatPixelHitManager::rebuildGeometry);
  // First update
  sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    return Error("Failed to update station structure.");
  }

  // Invalidate measurements at the beginning of each event
  incSvc()->addListener(this, IncidentType::BeginEvent);

  // Setup the hit pool.
  m_pool.resize(10000);
  m_nextInPool = m_pool.begin();
  m_maxSize = 0;
  m_eventReady = false;
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Finalize method.
//=========================================================================
StatusCode PatPixelHitManager::finalize() {

  info() << "Maximum number of Velo hits " << m_maxSize << endmsg;
  std::vector<PatPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) delete *itm;
  }
  return GaudiTool::finalize();

}

//=========================================================================
// Rebuild the geometry. Needed in case something changes in the Velo during the run...
//=========================================================================
StatusCode PatPixelHitManager::rebuildGeometry() {

  // Delete the existing modules.
  std::vector<PatPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) delete *itm;
  }
  m_modules.clear();
  m_firstModule = 999;
  m_lastModule  = 0;

  int previousLeft = -1;
  int previousRight = -1;
  std::vector<DeVPSensor*>::const_iterator its;
  for (its = m_vp->sensorsBegin(); m_vp->sensorsEnd() > its; ++its) {
    //== TO BE DONE === 
    // if (!(*its)->isReadOut()) continue;
    // Get the number of the module this sensor is on.
    const unsigned int number = (*its)->module();
    if (number < m_modules.size()) {
      // Check if this module has already been setup.
      if (m_modules[number]) continue;
    } else {
      while (number > m_modules.size() + 1) {
        m_modules.push_back(0);
      }
    }
    // Create a new module and add it to the list.
    PatPixelModule* module = new PatPixelModule(number, (*its)->isRight());
    module->setZ((*its)->z());
    if ((*its)->isRight()) {
      module->setPrevious(previousRight);
      previousRight = number;
    } else {
      module->setPrevious(previousLeft);
      previousLeft = number;
    }
    m_modules.push_back(module);
    if (m_firstModule > number) m_firstModule = number;
    if (m_lastModule  < number) m_lastModule  = number;
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Found modules from " << m_firstModule << " to " << m_lastModule << endmsg;
    for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
      if (*itm) {
        debug() << "  Module " << (*itm)->number() << " prev " << (*itm)->previous() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;

}

//=========================================================================
// Incident handler
//=========================================================================
void PatPixelHitManager::handle(const Incident& incident) {
  if (IncidentType::BeginEvent == incident.type()) {
    this->clearHits();
  }
}

//=========================================================================
// Clear all the hits from a previous event
//=========================================================================
void PatPixelHitManager::clearHits() {
  int lastSize = m_nextInPool - m_pool.begin();
  if (lastSize > m_maxSize) m_maxSize = lastSize;
  std::vector<PatPixelModule*>::iterator itS;
  for (itS = m_modules.begin(); m_modules.end() != itS; ++itS) {
    if (*itS) (*itS)->reset();
  }
  m_nextInPool = m_pool.begin();
  m_eventReady = false;
}

//=========================================================================
//  Convert the LiteClusters to PatPixelHits
//=========================================================================
void PatPixelHitManager::buildHits() {

  if (m_eventReady) return;
  m_eventReady = true;

  // Get the clusters.
  LHCb::VPLiteCluster::VPLiteClusters* liteClusters =
    GaudiTool::get<LHCb::VPLiteCluster::VPLiteClusters>(LHCb::VPLiteClusterLocation::Default);
  // If necessary adjust the size of the hit pool.
  if (liteClusters->size() > m_pool.size()) {
    m_pool.resize(liteClusters->size() + 100);
    m_nextInPool = m_pool.begin();
  }
  // Assume binary resolution of hit position.
  const double dx = 0.055 / sqrt(12.0);
  // Loop over clusters.
  LHCb::VPLiteCluster::VPLiteClusters::iterator itc; 
  for (itc = liteClusters->begin(); liteClusters->end() != itc; ++itc) {
    const unsigned int module = itc->channelID().module();
    if (module >= m_modules.size()) break;
    // Get the next object in the pool => here we store the new hit
    PatPixelHit* hit = &(*(m_nextInPool++));
    // Calculate the 3-D position for this cluster.
    Gaudi::XYZPoint point = position((*itc).channelID(),
                                     (*itc).interPixelFractionX(), 
                                     (*itc).interPixelFractionY());
    // Set our hit data
    hit->setHit(LHCb::LHCbID((*itc).channelID()), point, dx, dx, module);
    m_modules[module]->addHit(hit);
  }

}

//=========================================================================
// Calculate global position of a cluster.
//=========================================================================
Gaudi::XYZPoint PatPixelHitManager::position(LHCb::VPChannelID id, double dx, double dy) {

  const DeVPSensor* sensor = m_vp->sensorOfChannel(id);
  std::pair<double, double> offsets(dx, dy);
  Gaudi::XYZPoint point = sensor->channelToPoint(id, offsets);
  // If no correction to be applied, we're done.
  if (!m_useSlopeCorrection) return point;
  double dx_prime = dx, dy_prime = dy;
  Double_t delta_x = fabs(dx - 0.5);
  Double_t delta_y = fabs(dy - 0.5);
  if (dx == 0.5 && dy == 0.5) return point;
  if (dx != 0.5) {
    Double_t slx = fabs(point.x() / point.z());
    Double_t p_offset = 0.31172471 + 0.15879833 * TMath::Erf(-6.78928312 * slx + 0.73019077);
    Double_t t_factor = 0.43531842 + 0.3776611 * TMath::Erf(6.84465914 * slx - 0.75598833);
    dx_prime = 0.5 + (dx - 0.5) / delta_x * (p_offset + t_factor * delta_x); 
  }
  if (dy != 0.5) {
    Double_t sly = fabs(point.y() / point.z());
    Double_t p_offset = 0.35829374 - 0.20900493 * TMath::Erf(5.67571733 * sly -0.40270243);
    Double_t t_factor = 0.29798696 + 0.47414641 * TMath::Erf(5.84419802 * sly -0.40472057);
    dy_prime = 0.5 + (dy - 0.5) / delta_y * (p_offset + t_factor * delta_y);
  }
  std::pair<double, double> offsets2(dx_prime, dy_prime);
  return sensor->channelToPoint(id, offsets2);

}

//=========================================================================
// Sort hits by X within every module to speed up the track search
//=========================================================================
void PatPixelHitManager::sortByX() {
  std::vector<PatPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) {
      std::sort((*itm)->hits().begin(), (*itm)->hits().end(), 
                PatPixelHit::LowerByX()); 
    }
  }
}
