#include "TMath.h"
// Gaudi
#include "GaudiKernel/ToolFactory.h"

// Local
#include "PrPixelHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrPixelHitManager
//
// 2012-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(PrPixelHitManager)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelHitManager::PrPixelHitManager(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent) :
    GaudiTool(type, name, parent), m_useSlopeCorrection(true) {

  declareInterface<PrPixelHitManager>(this);

}

//=============================================================================
// Destructor
//=============================================================================
PrPixelHitManager::~PrPixelHitManager() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrPixelHitManager::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  // Get detector element.
  m_vp = getDet<DeVP>(DeVPLocation::Default);
  // Make sure we are up-to-date on populated VELO stations
  registerCondition((*(m_vp->leftSensorsBegin()))->geometry(), &PrPixelHitManager::rebuildGeometry);
  registerCondition((*(m_vp->rightSensorsBegin()))->geometry(), &PrPixelHitManager::rebuildGeometry);
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
  m_eventReady = false;
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Finalize method.
//=========================================================================
StatusCode PrPixelHitManager::finalize() {

  // Delete the PrPixelModule instances.
  std::vector<PrPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) delete *itm;
  }
  return GaudiTool::finalize();

}

//=========================================================================
// Rebuild the geometry. Needed in case something changes in the Velo during the run...
//=========================================================================
StatusCode PrPixelHitManager::rebuildGeometry() {

  // Delete the existing modules.
  std::vector<PrPixelModule*>::iterator itm;
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
    PrPixelModule* module = new PrPixelModule(number, (*its)->isRight());
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
void PrPixelHitManager::handle(const Incident& incident) {
  if (IncidentType::BeginEvent == incident.type()) {
    this->clearHits();
  }
}

//=========================================================================
// Clear all the hits from a previous event
//=========================================================================
void PrPixelHitManager::clearHits() {
  std::vector<PrPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) (*itm)->reset();
  }
  m_nextInPool = m_pool.begin();
  m_eventReady = false;
}

//=========================================================================
//  Convert the LiteClusters to PrPixelHits
//=========================================================================
void PrPixelHitManager::buildHits() {

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
  LHCb::VPLiteCluster::VPLiteClusters::const_iterator itc; 
  LHCb::VPLiteCluster::VPLiteClusters::const_iterator itc_end(liteClusters->end());
  for (itc = liteClusters->begin(); itc_end != itc; ++itc) {
    const unsigned int module = itc->channelID().module();
    if (module >= m_modules.size()) break;
    // Get the next object in the pool => here we store the new hit
    PrPixelHit* hit = &(*(m_nextInPool++));
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
Gaudi::XYZPoint PrPixelHitManager::position(LHCb::VPChannelID id, double dx, double dy) {

  const DeVPSensor* sensor = m_vp->sensorOfChannel(id);
  std::pair<double, double> offsets(dx, dy);
  Gaudi::XYZPoint point = sensor->channelToPoint(id, offsets);
  // If no correction to be applied, we're done.
  if (!m_useSlopeCorrection) return point;
  double dx_prime = dx, dy_prime = dy;
  double delta_x = fabs(dx - 0.5);
  double delta_y = fabs(dy - 0.5);
  if (dx == 0.5 && dy == 0.5) return point;
  if (dx != 0.5) {
    double slx = fabs(point.x() / point.z());
    double p_offset = 0.31172471 + 0.15879833 * TMath::Erf(-6.78928312 * slx + 0.73019077);
    double t_factor = 0.43531842 + 0.3776611 * TMath::Erf(6.84465914 * slx - 0.75598833);
    dx_prime = 0.5 + (dx - 0.5) / delta_x * (p_offset + t_factor * delta_x); 
  }
  if (dy != 0.5) {
    double sly = fabs(point.y() / point.z());
    double p_offset = 0.35829374 - 0.20900493 * TMath::Erf(5.67571733 * sly -0.40270243);
    double t_factor = 0.29798696 + 0.47414641 * TMath::Erf(5.84419802 * sly -0.40472057);
    dy_prime = 0.5 + (dy - 0.5) / delta_y * (p_offset + t_factor * delta_y);
  }
  std::pair<double, double> offsets2(dx_prime, dy_prime);
  return sensor->channelToPoint(id, offsets2);

}

//=========================================================================
// Sort hits by X within every module to speed up the track search
//=========================================================================
void PrPixelHitManager::sortByX() {
  std::vector<PrPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) {
      if (!((*itm)->empty())) {
        std::sort((*itm)->hits().begin(), (*itm)->hits().end(), 
                  PrPixelHit::LowerByX());
        (*itm)->setLastHitX((*itm)->hits().back()->x());
      }
    }
  }
}
